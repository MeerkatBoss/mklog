#include "mklog/LogManager.h"

#include <bits/types/sigset_t.h>
#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include "mklog/LogWriter.h"
#include "mklog/utils/SimpleList.h"

namespace mklog
{

utils::SimpleList<LogWriter*> LogManager::s_writerList =
    utils::SimpleList<LogWriter*>();

utils::SimpleList<LogManager::LongMessageInfo> LogManager::s_longMsgList =
    utils::SimpleList<LogManager::LongMessageInfo>();

utils::SimpleList<LogManager::HandledSignal> LogManager::s_handledSignals =
    utils::SimpleList<LogManager::HandledSignal>();

LogManager::Status LogManager::s_currentStatus =
    LogManager::Status::UNINITIALIZED;

void LogManager::handleSignal(int signumber)
{
  // Try to send all messages
  for (LongMessageInfo& messageInfo : s_longMsgList)
  {
    size_t appendedLen = appendPipeContent(&messageInfo);
    if (appendedLen > 0)
    {
      // Send message
      logMessage(messageInfo.message);

      // Make empty content
      char* newContent = new char[2];
      newContent[0]    = '\n';
      newContent[1]    = '\0';
      delete[] messageInfo.message.content;
      messageInfo.message.content    = newContent;
      messageInfo.message.contentLen = 2;
    }
  }

  // Find old handler
  const struct sigaction* oldHandler = nullptr;
  for (const HandledSignal& handledSignal : s_handledSignals)
  {
    if (handledSignal.signal == signumber)
    {
      oldHandler = &handledSignal.prevAction;
      break;
    }
  }

  // If found old handler
  if (oldHandler != nullptr)
  {
    struct sigaction curHandler = {};
    sigset_t         procMask   = {};

    // Unblock signal
    sigemptyset(&procMask);
    sigaddset(&procMask, signumber);
    sigprocmask(SIG_UNBLOCK, &procMask, NULL);

    // Call old handler
    sigaction(signumber, oldHandler, &curHandler);
    raise(signumber);
    sigaction(signumber, &curHandler, NULL);
  }
}

size_t LogManager::appendPipeContent(LogManager::LongMessageInfo* messageInfo)
{
  LogMessage& curMessage = messageInfo->message;
  MessageFd   readFd     = messageInfo->contentReadFd;

  errno = 0;

  // Read all pipe content
  const size_t maxContentSize = curMessage.contentLen + LONG_MESSAGE_LEN_MAX;
  char*        fullContent    = new char[maxContentSize];
  char*        appendStart    = fullContent + curMessage.contentLen - 1;
  ssize_t      appendSize     = read(readFd, appendStart, LONG_MESSAGE_LEN_MAX);
  assert((appendSize != -1 || errno == EAGAIN) && "Read from pipe failed");

  // If nothing is read
  if (appendSize <= 0)
  {
    delete[] fullContent;
    return 0;
  }

  // Prepend template content to pipe content
  strncpy(fullContent, curMessage.content, curMessage.contentLen - 1);

  // Replace old content with new
  delete[] curMessage.content;
  curMessage.content = fullContent;

  // Adjust contentLen
  curMessage.contentLen += appendSize - 1;

  return appendSize;
}

void LogManager::endLogs()
{
  // Check that logs are started
  assert(s_currentStatus == Status::READY &&
         "Cannot end logs: logs not started");

  // For all registered long messages
  for (LongMessageInfo& messageInfo : s_longMsgList)
  {
    // Read all pipe content
    size_t appendedLen = appendPipeContent(&messageInfo);
    if (appendedLen)
    {
      // Send message to all writers
      logMessage(messageInfo.message);
    }

    // Dispose message content
    delete[] messageInfo.message.content;
  }
  s_longMsgList.clear();

  // For all writers
  for (LogWriter* writer : s_writerList)
  {
    // Delete writer
    delete writer;
  }
  // Clear list of writers
  s_writerList.clear();

  // Mark LogManager as deinitialized
  s_currentStatus = Status::UNINITIALIZED;
} // namespace mklog

void LogManager::initLogs()
{
  // Check that logs are not started
  assert(s_currentStatus == Status::UNINITIALIZED &&
         "Cannot start logs: logs already started");

  // Register exit callbacks
  atexit(&LogManager::endLogs);

  // Register signal handlers
  for (int signum : SIGNALS_TO_HANDLE)
  {
    struct sigaction prevAction = {}, newAction = {};

    newAction.sa_handler = &handleSignal;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(signum, &newAction, &prevAction);
    s_handledSignals.pushFront({.signal = signum, .prevAction = prevAction});
  }

  // Mark LogManager as ready
  s_currentStatus = Status::READY;
}

void LogManager::logMessage(const LogMessage& message)
{
  // Check that LogManager is ready
  if (s_currentStatus != Status::READY)
  {
    return;
  }

  // For each registered writer
  for (LogWriter* writer : s_writerList)
  {
    // Try to send message to writer
    writer->tryWriteMessage(message);
  }
}

LogManager::MessageFd LogManager::beginLongMessage(const LogMessage& message)
{
  if (s_currentStatus != Status::READY)
  {
    return STDERR_FILENO;
  }

  MessageFd pipeFds[2] = {MESSAGE_FD_INVALID, MESSAGE_FD_INVALID};

  // Open pipe for message content
  int pipeStatus = pipe(pipeFds);
  if (pipeStatus != 0) // Failed to create pipe
  {
    return MESSAGE_FD_INVALID;
  }
  MessageFd pipeReadFd  = pipeFds[0];
  MessageFd pipeWriteFd = pipeFds[1];

  // Ensure correct buffer size for pipe
  fcntl(pipeReadFd, F_SETPIPE_SZ, (int)LONG_MESSAGE_LEN_MAX);

  // Make read nonblocking
  int oldFlags = fcntl(pipeReadFd, F_GETFL);
  fcntl(pipeReadFd, F_SETFL, oldFlags | O_NONBLOCK);

  // Copy template content
  char* contentCopy = new char[message.contentLen];
  memcpy(contentCopy, message.content, message.contentLen);

  // Create message for saving
  LogMessage messageCopy = message;
  messageCopy.content    = contentCopy;

  // Register long message
  s_longMsgList.pushFront({.message        = messageCopy,
                           .contentWriteFd = pipeWriteFd,
                           .contentReadFd  = pipeReadFd});

  // Return opened pipe write file descriptor
  return pipeWriteFd; // NOLINT: no memory leak, contentCopy is saved
}

void LogManager::endLongMessage(LogManager::MessageFd& fd)
{
  if (s_currentStatus != Status::READY)
  {
    return;
  }

  // Check input fd
  assert(fd != MESSAGE_FD_INVALID && "Attempted access with invalid fd");

  // Try to find message associated with file descriptor
  LongMessageInfo* foundMessageInfo = nullptr;
  for (LongMessageInfo& messageInfo : s_longMsgList)
  {
    if (messageInfo.contentWriteFd == fd)
      foundMessageInfo = &messageInfo;
  }

  // Check that message is found
  assert(foundMessageInfo != nullptr && "Unknown fd used to access message");

  // Build message content
  size_t appendedLen = appendPipeContent(foundMessageInfo);

  if (appendedLen > 0)
  {
    // Send message to all writers
    logMessage(foundMessageInfo->message);
  }

  // Save pipe descriptors
  MessageFd readFd  = foundMessageInfo->contentReadFd;
  MessageFd writeFd = foundMessageInfo->contentWriteFd;

  // Save message content
  const char* messageContent = foundMessageInfo->message.content;

  // Remove message from list
  for (auto it = s_longMsgList.begin(), end = s_longMsgList.end(); it != end;
       ++it)
  {
    if ((*it).contentWriteFd == fd)
    {
      s_longMsgList.erase(it);
      break;
    }
  }

  // Close pipe descriptors
  close(readFd);
  close(writeFd);

  // Delete content
  delete[] messageContent;

  // Invalidate input descriptor
  fd = MESSAGE_FD_INVALID;
}

} // namespace mklog

/**
 * @file LogManager.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Main class for logging
 *
 * @version 0.1
 * @date 2023-08-13
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGMANAGER_H
#define __MEERKAT_LOGS_LOGMANAGER_H

#include <csignal>
#include <signal.h>

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogWriter.h"
#include "meerkat_logs/utils/SimpleList.h"

namespace meerkat_logs
{

/**
 * @brief Main class used for logging. Manages all LogWriters. Accepts and
 * distributes log messages.
 */
class LogManager
{
public:
  /**
   * @brief File descriptor for long message content
   */
  using MessageFd = int;

  /**
   * @brief Maximum number of characters to append to long message content
   */
  static constexpr size_t LONG_MESSAGE_LEN_MAX = 4096;

  static constexpr MessageFd MESSAGE_FD_INVALID = -1;

private:
  /**
   * @brief List of all registered LogWriters
   */
  static utils::SimpleList<LogWriter*> s_writerList;

  /**
   * @brief Description of started long message
   */
  struct LongMessageInfo
  {
    LogMessage message;
    MessageFd  contentWriteFd;
    MessageFd  contentReadFd;
  };

  static utils::SimpleList<LongMessageInfo> s_longMsgList;

  /**
   * @brief State of LogManager
   */
  enum class Status
  {
    UNINITIALIZED,
    READY,
    DEINITIALIZED,
  };

  /**
   * @brief Current state of LogManager
   */
  static Status s_currentStatus;

  /**
   * @brief Signal handled by log manager
   */
  struct HandledSignal
  {
    int              signal;
    struct sigaction prevAction;
  };

  /**
   * @brief List of all handled signals
   */
  static utils::SimpleList<HandledSignal> s_handledSignals;

  /**
   * @brief All signals LogManager should handle
   */
  static constexpr int SIGNALS_TO_HANDLE[] = {SIGFPE,  SIGILL,  SIGSEGV, SIGBUS,
                                              SIGABRT, SIGSYS,  SIGTERM, SIGINT,
                                              SIGQUIT, SIGKILL, SIGHUP};

  /**
   * @brief Handler registered for all `SIGNALS_TO_HANDLE`
   */
  static void handleSignal(int signumber);

  /**
   * @brief Read all long message content and save it
   *
   * @param[inout] messageInfo	Long message to read content of
   *
   * @return Length of appended content
   */
  static size_t appendPipeContent(LongMessageInfo* messageInfo);

  /**
   * @brief End all logging. Invalidate LogManager
   */
  static void endLogs();

public:
  // Forbid construction of static class
  LogManager() = delete;

  /**
   * @brief Register new LogWriter in LogManager
   *
   * @tparam TWriter  LogWriter implementation
   *
   * @param[in] ...	  TWriter constructor arguments
   */
  template <typename TWriter, typename... TArgs>
  static TWriter& addWriter(TArgs... args)
  {
    TWriter* writer = new TWriter(args...);
    s_writerList.pushFront(writer);
    return *writer;
  }

  /**
   * @brief Initialize logging for program
   */
  static void initLogs();

  /**
   * @brief Send log message to all registered writers
   *
   * @param[in] message	  Log message to be sent
   */
  static void logMessage(const LogMessage& message);

  /**
   * @brief Create file descriptor for new long message. Anything written
   * to returned `MessageFd` will be appended to `messageTemplate.content`.
   * If nothing is written to returned `MessageFd`, no message will be
   * written. Message is not guaranteed to be printed all at once, it may be
   * split into several messages. Total length of appended content for long
   * message may not exceed `LogManager::LONG_MESSAGE_LEN_MAX`.
   *
   * @param[in] messageTemplate	  Template for long message
   *
   * @return Upon success, return file descriptor for message content
   *         If error occured while creating long message, return
   *         `LogManager::MESSAGE_FD_INVALID`
   *
   * @warning  Any attempt to write more than `LogManager::LONG_MESSAGE_LEN_MAX`
   * to returned `MessageFd` results in undefined behaviour.
   */
  static MessageFd beginLongMessage(const LogMessage& messageTemplate);

  /**
   * @brief End long message with file descriptor `fd`. If any content has
   * been appended, the message is guaranteed to be printed after call to this
   * function.
   *
   * @param[inout] fd     Long message file descriptor
   *
   * @warning `fd` is invalidated after call to this function. Do not use it
   * after call
   */
  static void endLongMessage(MessageFd& fd);
};

} // namespace meerkat_logs

#endif /* LogManager.h */

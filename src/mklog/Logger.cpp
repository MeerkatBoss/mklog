#include "mklog/Logger.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>

#include "mklog/LogManager.h"

namespace mklog // TODO: Maybe meerkat::logs?
{

void Logger::logMessage(MessageSeverity severity, MessageSource source,
                        MessageContentType contentType, const char* format, ...)
{
  // Get message timestamp
  const time_t timestamp = time(NULL);

  // Fill information about source logger
  source.logger = loggerName;

  // Calculate message content length
  va_list args = {};
  va_start(args, format);
  // Add 1 for NUL terminator -------------------------------v
  const size_t contentLen = vsnprintf(NULL, 0, format, args) + 1;
  va_end(args);

  // Produce message content
  char* messageContent = new char[contentLen]; // TODO: Reuse memory
  va_start(args, format);
  vsnprintf(messageContent, contentLen, format, args);
  va_end(args);

  // Construct LogMessage
  LogMessage message = {.severity    = severity,
                        .source      = source,
                        .contentType = contentType,
                        .content     = messageContent,
                        .contentLen  = contentLen,
                        .timestamp   = timestamp};

  // Send LogMessage through LogManager
  LogManager::logMessage(message);

  // Dispose message content
  delete[] messageContent;
}

LogManager::MessageFd Logger::beginLongMessage(MessageSeverity    severity,
                                               MessageSource      source,
                                               MessageContentType contentType,
                                               const char*        format, ...)
{
  // Get message timestamp
  const time_t timestamp = time(NULL);

  // Fill information about source logger
  source.logger = loggerName;

  // Calculate message content length
  va_list args = {};
  va_start(args, format);
  // Add 2 for NUL terminator and LF ------------------------v
  const size_t contentLen = vsnprintf(NULL, 0, format, args) + 2;
  va_end(args);

  // Produce message content
  char* messageContent = new char[contentLen];
  va_start(args, format);
  vsnprintf(messageContent, contentLen, format, args);
  va_end(args);
  // Add LF
  messageContent[contentLen - 2] = '\n';
  messageContent[contentLen - 1] = '\0';

  // Construct LogMessage
  LogMessage message = {.severity    = severity,
                        .source      = source,
                        .contentType = contentType,
                        .content     = messageContent,
                        .contentLen  = contentLen,
                        .timestamp   = timestamp};

  // Register long message
  LogManager::MessageFd messageFd = LogManager::beginLongMessage(message);

  // Dispose message content
  delete[] messageContent;

  return messageFd;
}

void Logger::endLongMessage(LogManager::MessageFd& messageFd)
{
  LogManager::endLongMessage(messageFd);
}
} // namespace mklog

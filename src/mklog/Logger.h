/**
 * @file Logger.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Main interface for logging
 *
 * @version 0.1
 * @date 2023-08-13
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGGER_H
#define __MEERKAT_LOGS_LOGGER_H

#include <cstring>

#include "mklog/LogManager.h"

namespace mklog
{

class Logger
{
private:
  char* loggerName;

public:
  static constexpr size_t NAME_LEN_MAX = 128;

  Logger(const char* name)
  {
    size_t nameLen = strnlen(name, NAME_LEN_MAX);

    loggerName = new char[nameLen + 1];

    strncpy(loggerName, name, nameLen);
    loggerName[nameLen] = 0;
  }

  // No copying
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  /**
   * @brief Issue new log message. Cannot be called directly, use LOG_* macros
   * instead.
   *
   * @param[in] severity	  Log message severity
   * @param[in] source	    Log message source
   * @param[in] contentType Log message content type
   * @param[in] format	    Log message printf format string
   * @param[in] ...	        Log message printf format arguments
   */
  void logMessage(MessageSeverity severity, MessageSource source,
                  MessageContentType contentType, const char* format, ...)
      __attribute__((__format__(__printf__, 5, 6)));

  /**
   * @brief Register new long message. Cannot be called directly, use
   * LOG_BEGIN_* macros instead.
   *
   * @param[in] severity	  Log message severity
   * @param[in] source	    Log message source
   * @param[in] contentType Log message content type
   * @param[in] format	    Log message printf format string
   * @param[in] ...	        Log message printf format arguments
   *
   * @return File descriptor for long message content
   */
  LogManager::MessageFd
  beginLongMessage(MessageSeverity severity, MessageSource source,
                   MessageContentType contentType, const char* format, ...)
      __attribute__((__format__(__printf__, 5, 6)));

  /**
   * @brief End long message. Invalidate `messageFd`
   *
   * @param[inout] messageFd	Content file descriptor for long message
   */
  void endLongMessage(LogManager::MessageFd& messageFd);

#define __LOG_MESSAGE(severity, type, ...)                                     \
  logMessage(severity,                                                         \
             {.file     = __FILE__,                                            \
              .function = __PRETTY_FUNCTION__,                                 \
              .line     = __LINE__,                                            \
              .logger   = nullptr},                                              \
             type, __VA_ARGS__)

#define __LOG_BEGIN(severity, type, ...)                                       \
  beginLongMessage(severity,                                                   \
                   {.file     = __FILE__,                                      \
                    .function = __PRETTY_FUNCTION__,                           \
                    .line     = __LINE__,                                      \
                    .logger   = nullptr},                                        \
                   type, __VA_ARGS__)

/**
 * @brief Issue new log message with severity TRACE
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_TRACE(...)                                                         \
  __LOG_MESSAGE(mklog::MessageSeverity::TRACE, __VA_ARGS__)

/**
 * @brief Issue new log message with severity DEBUG
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_DEBUG(...)                                                         \
  __LOG_MESSAGE(mklog::MessageSeverity::DEBUG, __VA_ARGS__)

/**
 * @brief Issue new log message with severity INFO
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_INFO(...)                                                          \
  __LOG_MESSAGE(mklog::MessageSeverity::INFO, __VA_ARGS__)

/**
 * @brief Issue new log message with severity WARNING
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_WARNING(...)                                                       \
  __LOG_MESSAGE(mklog::MessageSeverity::WARNING, __VA_ARGS__)

/**
 * @brief Issue new log message with severity ERROR
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_ERROR(...)                                                         \
  __LOG_MESSAGE(mklog::MessageSeverity::ERROR, __VA_ARGS__)

/**
 * @brief Issue new log message with severity FATAL
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_FATAL(...)                                                         \
  __LOG_MESSAGE(mklog::MessageSeverity::FATAL, __VA_ARGS__)

/**
 * @brief Register new long message with severity TRACE
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 *
 * @return File descriptor for long message content
 */
#define LOG_BEGIN_TRACE(...)                                                   \
  __LOG_BEGIN(mklog::MessageSeverity::TRACE, __VA_ARGS__)

/**
 * @brief Register new long message with severity DEBUG
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 *
 * @return File descriptor for long message content
 */
#define LOG_BEGIN_DEBUG(...)                                                   \
  __LOG_BEGIN(mklog::MessageSeverity::DEBUG, __VA_ARGS__)

/**
 * @brief Register new long message with severity INFO
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 *
 * @return File descriptor for long message content
 */
#define LOG_BEGIN_INFO(...)                                                    \
  __LOG_BEGIN(mklog::MessageSeverity::INFO, __VA_ARGS__)

/**
 * @brief Register new long message with severity WARNING
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 *
 * @return File descriptor for long message content
 */
#define LOG_BEGIN_WARNING(...)                                                 \
  __LOG_BEGIN(mklog::MessageSeverity::WARNING, __VA_ARGS__)

/**
 * @brief Register new long message with severity ERROR
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 *
 * @return File descriptor for long message content
 */
#define LOG_BEGIN_ERROR(...)                                                   \
  __LOG_BEGIN(mklog::MessageSeverity::ERROR, __VA_ARGS__)

/**
 * @brief Register new long message with severity FATAL
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 *
 * @return File descriptor for long message content
 */
#define LOG_BEGIN_FATAL(...)                                                   \
  __LOG_BEGIN(mklog::MessageSeverity::FATAL, __VA_ARGS__)

  ~Logger() { delete[] loggerName; }
};

} // namespace mklog

#endif /* Logger.h */

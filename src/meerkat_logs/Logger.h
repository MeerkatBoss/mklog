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

#include "meerkat_logs/LogManager.h"

namespace meerkat_logs
{

using MessageContentType = LogMessage::ContentType;
using MessageSeverity    = LogMessage::Severity;
using MessageSource      = LogMessage::Source;

class Logger
{
private:
  char* loggerName;

public:
  static constexpr size_t MAX_LOGGER_NAME_LEN = 128;

  Logger(const char* name)
  {
    size_t nameLen = strnlen(name, MAX_LOGGER_NAME_LEN);

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

#define __LOG_MESSAGE(severity, type, ...)                                     \
  logMessage(severity,                                                         \
             {.file     = __FILE__,                                            \
              .function = __PRETTY_FUNCTION__,                                 \
              .line     = __LINE__,                                            \
              .logger   = nullptr},                                              \
             type, __VA_ARGS__)

/**
 * @brief Issue new log message with severity TRACE
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_TRACE(...) __logMessage(Severity::TRACE, ...)

/**
 * @brief Issue new log message with severity DEBUG
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_DEBUG(...) __logMessage(Severity::DEBUG, ...)

/**
 * @brief Issue new log message with severity INFO
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_INFO(...) __logMessage(Severity::INFO, ...)

/**
 * @brief Issue new log message with severity WARNING
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_WARNING(...) __logMessage(Severity::WARNING, ...)

/**
 * @brief Issue new log message with severity ERROR
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_ERROR(...) __logMessage(Severity::ERROR, ...)

/**
 * @brief Issue new log message with severity FATAL
 *
 * @param[in] contentType Log message content type
 * @param[in] format	    Log message printf format string
 * @param[in] ...	        Log message printf format arguments
 */
#define LOG_FATAL(...) __logMessage(Severity::FATAL, ...)

  ~Logger() { delete loggerName; }
};

} // namespace meerkat_logs

#endif /* Logger.h */

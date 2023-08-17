/**
 * @file LogMessage.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Main log message structure
 *
 * @version 0.1
 * @date 2023-08-12
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGMESSAGE_H
#define __MEERKAT_LOGS_LOGMESSAGE_H

#include <cstddef>
#include <ctime>

namespace meerkat_logs
{

/**
 * @brief Message created by logger
 */
struct LogMessage
{
  /**
   * @brief Severity/importance level of log message
   */
  enum class Severity
  {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,

    MIN_LEVEL = TRACE,
    MAX_LEVEL = FATAL,
  };

  /**
   * @brief Type of content stored in log message
   */
  enum class ContentType
  {
    TEXT,
    CODE,
    IMAGE,
  };

  /**
   * @brief Log message source description
   */
  struct Source
  {
    const char* file;
    const char* function;
    size_t      line;
    const char* logger;
  };

  Severity    severity;
  Source      source;
  ContentType contentType;

  const char* content;
  time_t      timestamp;
};

} // namespace meerkat_logs

#endif /* LogMessage.h */

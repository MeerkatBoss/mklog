/**
 * @file LogWriter.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Writer of log messages
 *
 * @version 0.1
 * @date 2023-08-12
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGWRITER_H
#define __MEERKAT_LOGS_LOGWRITER_H

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogRoute.h"

namespace meerkat_logs
{

/**
 * @brief Base class for log printers
 */
class LogWriter
{
public:
  /**
   * @brief Status of writing log message
   *
   */
  enum class Status
  {
    OK,                       /// Message successfully written
    CONTENT_TYPE_NOT_ALLOWED, /// Incompatible message content type
    ROUTE_NO_MATCH,           /// Message not accepted by routing rules
  };

private:
  LogRoute route;

  /**
   * @brief Check if message matches routing rules defined by route
   *
   * @param[in] message	  Message to be matched
   *
   * @return `true` if message matches routing rules, `false` otherwise
   */
  bool matchMessage(const LogMessage& message)
  {
    return route.matchMessage(message);
  }

protected:
  /**
   * @brief Check if content type is acceptable
   *
   * @param[in] contentType	  Message content type
   *
   * @return `true` if message with this type can be written, `false` otherwise
   */
  virtual bool
  canAcceptContentType(LogMessage::ContentType contentType) const = 0;

  /**
   * @brief Write log message
   *
   * @param[in] message	  Message to be written
   *
   * @return `LogWriter::Status::OK`
   */
  virtual Status writeMessage(const LogMessage& message) = 0;

  LogWriter() : route(LogRoute::makeRoute<DefaultRoutingRule>()) {}

public:
  /**
   * @brief Write log message if it matches routing rules for this LogWriter
   *
   * @param[in] message	  Message to be written
   *
   * @return Status of printing message
   */
  Status tryWriteMessage(const LogMessage& message)
  {
    if (!matchMessage(message))
    {
      return Status::ROUTE_NO_MATCH;
    }
    if (!canAcceptContentType(message.contentType))
    {
      return Status::CONTENT_TYPE_NOT_ALLOWED;
    }

    return writeMessage(message);
  }

  LogWriter& setRoute(const LogRoute& route)
  {
    this->route = route;
    return *this;
  }

  virtual ~LogWriter() = default;
};

} // namespace meerkat_logs

#endif /* LogWriter.h */

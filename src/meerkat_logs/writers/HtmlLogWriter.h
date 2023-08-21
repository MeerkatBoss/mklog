/**
 * @file HtmlLogWriter.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Log writer for HTML file format
 *
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_WRITERS_HTMLLOGWRITER_H
#define __MEERKAT_LOGS_WRITERS_HTMLLOGWRITER_H

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogWriter.h"

namespace meerkat_logs
{

/**
 * @brief Writes logs in HTML format. Uses the following CSS classes:
 *   - 'message'    - Log message
 *   - 'timestamp'  - Message timestamp
 *   - 'severity'   - Message severity
 *     - 'trace'    - TRACE severity
 *     - 'debug'    - DEBUG severity
 *     - 'info'     - INFO severity
 *     - 'warning'  - WARNING severity
 *     - 'error'    - ERROR severity
 *     - 'fatal'    - FATAL severity
 *   - 'source'     - Message source
 *   - 'text'       - Message TEXT content
 *
 *   CODE content is placed inside <code> tag, IMAGE is placed in <img> tag
 */
class HtmlLogWriter : public LogWriter
{
private:
  int  logFd;
  bool isValid;

protected:
  bool canAcceptContentType(LogMessage::ContentType contentType) const override
  {
    return isValid && (contentType == LogMessage::ContentType::TEXT ||
                       contentType == LogMessage::ContentType::CODE ||
                       contentType == LogMessage::ContentType::IMAGE);
  }

  Status writeMessage(const LogMessage& message) override;

public:
  HtmlLogWriter() : LogWriter(), logFd(-1), isValid(false) {}

  HtmlLogWriter& setFile(const char* filename);

  bool valid() { return isValid; }
};

} // namespace meerkat_logs

#endif /* HtmlLogWriter.h */

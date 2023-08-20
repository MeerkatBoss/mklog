/**
 * @file TextLogWriter.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Log writer for text files
 *
 * @version 0.1
 * @date 2023-08-20
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_WRITERS_TEXTLOGWRITER_H
#define __MEERKAT_LOGS_WRITERS_TEXTLOGWRITER_H

#include <cassert>
#include <fcntl.h>

#include "meerkat_logs/LogWriter.h"

namespace meerkat_logs
{

/**
 * @brief Writes logs in plain text to specified file
 */
class TextLogWriter : public LogWriter
{
private:
  int  logFd;
  bool isValid;

protected:
  bool canAcceptContentType(LogMessage::ContentType contentType) const override
  {
    return isValid && contentType == LogMessage::ContentType::TEXT;
  }

  Status writeMessage(const LogMessage& message) override;

public:
  TextLogWriter() : LogWriter(), logFd(-1), isValid(false) {}

  TextLogWriter& setFile(const char* filename);

  bool valid() { return isValid; }
};

} // namespace meerkat_logs

#endif /* TextLogWriter.h */

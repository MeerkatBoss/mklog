/**
 * @file StderrLogWriter.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief LogWriter implementation for writing logs to stderr
 *
 * @version 0.1
 * @date 2023-08-17
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_WRITERS_STDERRLOGWRITER_H
#define __MEERKAT_LOGS_WRITERS_STDERRLOGWRITER_H

#include "mklog/LogMessage.h"
#include "mklog/LogRoute.h"
#include "mklog/LogWriter.h"
namespace mklog
{

/**
 * @brief Write logs to STDERR stream. Accept only plain text messages.
 */
class StderrLogWriter : public LogWriter
{
private:
  bool useEscapeCodes;

protected:
  bool canAcceptContentType(LogMessage::ContentType contentType) const override
  {
    // Accept text only
    return contentType == LogMessage::ContentType::TEXT;
  }

  Status writeMessage(const LogMessage& message) override;

public:
  StderrLogWriter(bool useEscapeCodes = false)
      : LogWriter(), useEscapeCodes(useEscapeCodes)
  {
  }

  StderrLogWriter& useAnsiColors()
  {
    useEscapeCodes = true;
    return *this;
  }
};

} // namespace mklog

#endif /* StderrLogWriter.h */

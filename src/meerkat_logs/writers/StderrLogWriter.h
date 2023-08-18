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

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogRoute.h"
#include "meerkat_logs/LogWriter.h"
namespace meerkat_logs
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

} // namespace meerkat_logs

#endif /* StderrLogWriter.h */

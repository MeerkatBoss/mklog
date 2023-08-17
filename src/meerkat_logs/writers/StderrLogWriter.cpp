#include "meerkat_logs/writers/StderrLogWriter.h"

#include <cstdio>

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogWriter.h"

namespace meerkat_logs
{

class EscapeCodes
{
public:
  EscapeCodes() = delete;

#define ESCAPE_CODE static constexpr const char*

  ESCAPE_CODE CLEAR      = "\033[0m";
  ESCAPE_CODE SET_FG     = "\033[38;5;";
  ESCAPE_CODE SET_BG     = "\033[48;5;";
  ESCAPE_CODE SET_BOLD   = "\033[1m";
  ESCAPE_CODE SET_ITALIC = "\033[3m";

  class Colors
  {
  public:
    Colors()               = delete;
    ESCAPE_CODE BLACK      = "0m";
    ESCAPE_CODE DARK_RED   = "1m";
    ESCAPE_CODE DARK_GREEN = "2m";
    ESCAPE_CODE BROWN      = "3m";
    ESCAPE_CODE DARK_BLUE  = "4m";
    ESCAPE_CODE PURPLE     = "5m";
    ESCAPE_CODE DARK_CYAN  = "6m";
    ESCAPE_CODE LIGHT_GRAY = "7m";
    ESCAPE_CODE DARK_GRAY  = "8m";
    ESCAPE_CODE RED        = "9m";
    ESCAPE_CODE GREEN      = "10m";
    ESCAPE_CODE YELLOW     = "11m";
    ESCAPE_CODE BLUE       = "12m";
    ESCAPE_CODE MAGENTA    = "13m";
    ESCAPE_CODE CYAN       = "14m";
    ESCAPE_CODE WHITE      = "15m";
  };

#undef ESCAPE_CODE
};

static const char* getSeverityColorCode(LogMessage::Severity severity)
{
  using Severity = LogMessage::Severity;

  switch (severity)
  {
  case Severity::TRACE:
    return EscapeCodes::Colors::LIGHT_GRAY;
  case Severity::DEBUG:
    return EscapeCodes::Colors::DARK_GRAY;
  case Severity::INFO:
    return EscapeCodes::Colors::DARK_BLUE;
  case Severity::WARNING:
    return EscapeCodes::Colors::BROWN;
  case Severity::ERROR:
    return EscapeCodes::Colors::RED;
  case Severity::FATAL:
    return EscapeCodes::Colors::MAGENTA;
  default:
    return EscapeCodes::Colors::BLACK;
  }
}

static const char* getSeverityString(LogMessage::Severity severity)
{
  using Severity = LogMessage::Severity;

  switch (severity)
  {
  case Severity::TRACE:
    return "trace";
  case Severity::DEBUG:
    return "debug";
  case Severity::INFO:
    return "info";
  case Severity::WARNING:
    return "WARNING";
  case Severity::ERROR:
    return "ERROR";
  case Severity::FATAL:
    return "!FATAL!";
  default:
    return "???";
  }
}

LogWriter::Status StderrLogWriter::writeMessage(const LogMessage& message)
{
  if (useEscapeCodes)
  {
    fprintf(stderr, "%s%s[%s]%s %.*s\n", EscapeCodes::SET_FG,
            getSeverityColorCode(message.severity),
            getSeverityString(message.severity), EscapeCodes::CLEAR,
            (int)message.contentLen, message.content);
  }
  else
  {
    fprintf(stderr, "[%s] %.*s\n", getSeverityString(message.severity),
            (int)message.contentLen, message.content);
  }
  return LogWriter::Status::OK;
}

} // namespace meerkat_logs

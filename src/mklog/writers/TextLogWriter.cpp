#include "mklog/writers/TextLogWriter.h"

#include <cassert>
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>

#include "mklog/LogWriter.h"

namespace mklog
{

TextLogWriter& TextLogWriter::setFile(const char* filename)
{
  assert(!isValid && "Cannot reset log file");

  int fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);

  if (fd >= 0)
  {
    logFd   = fd;
    isValid = true;
  }

  return *this;
}

static const char* getSeverityString(LogMessage::Severity severity)
{
  using Severity = LogMessage::Severity;

  switch (severity)
  {
  case Severity::TRACE:   return " TRACE ";
  case Severity::DEBUG:   return " DEBUG ";
  case Severity::INFO:    return " INFO  ";
  case Severity::WARNING: return "WARNING";
  case Severity::ERROR:   return " ERROR ";
  case Severity::FATAL:   return " FATAL ";
  default:                return "UNKNOWN";
  }
}

static const char* getTimeString(time_t timestamp)
{
  constexpr size_t      MAX_TIME_STR_LEN = 32;
  constexpr const char* TIME_STR_FORMAT  = "%F %T%z";

  static char s_timeStr[MAX_TIME_STR_LEN + 1] = "";

  struct tm* time = localtime(&timestamp);

  strftime(s_timeStr, MAX_TIME_STR_LEN, TIME_STR_FORMAT, time);

  return s_timeStr;
}

LogWriter::Status TextLogWriter::writeMessage(const LogMessage& message)
{
  assert(isValid && "Attempted write to invalid file");

  const char* time     = getTimeString(message.timestamp);
  const char* severity = getSeverityString(message.severity);
  dprintf(logFd, "<%s> [%s] '%s' in '%s' at '%s:%zu':\n\t%.*s\n", time, severity,
          message.source.logger, message.source.function, message.source.file,
          message.source.line, (int)message.contentLen, message.content);
  return Status::OK;
}

} // namespace mklog

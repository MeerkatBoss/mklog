#include "meerkat_logs/writers/HtmlLogWriter.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogWriter.h"

namespace meerkat_logs
{

static constexpr char PREAMBLE[] = "<body><pre>";

HtmlLogWriter& HtmlLogWriter::setFile(const char* filename)
{
  assert(!isValid && "Cannot reset log file");

  int fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);

  if (fd >= 0)
  {
    logFd   = fd;
    isValid = true;
    write(fd, PREAMBLE, sizeof(PREAMBLE) - 1);
  }

  return *this;
}

struct CharEscapeSeq
{
  char        toEscape;
  const char* escapeSeq;
  size_t      escapeSeqLen;
};

#define CHAR_ESCAPE_SEQ(ch, str)                                               \
  {                                                                            \
    .toEscape = ch, .escapeSeq = str, .escapeSeqLen = sizeof(str) - 1          \
  }

static constexpr CharEscapeSeq ESCAPED_CHARS[] = {
    CHAR_ESCAPE_SEQ('<', "&lt;"), CHAR_ESCAPE_SEQ('>', "&gt;"),
    CHAR_ESCAPE_SEQ('&', "&amp;")};

static bool needEscape(const char* content, size_t contentLen)
{
  // For each character in string
  for (size_t i = 0; i < contentLen; ++i)
  {
    // For each escape sequence
    for (const CharEscapeSeq& escape : ESCAPED_CHARS)
    {
      // If character needs to be escaped
      if (content[i] == escape.toEscape)
      {
        return true;
      }
    }
  }
  return false;
}

static size_t getEscapedLen(const char* content, size_t contentLen)
{
  size_t length = 0;
  // For each character in string
  for (size_t i = 0; i < contentLen; ++i)
  {
    bool isEscaped = false;
    // For each escape sequence
    for (const CharEscapeSeq& escape : ESCAPED_CHARS)
    {
      // If character needs to be escaped
      if (content[i] == escape.toEscape)
      {
        // Add escape sequence length to result
        length += escape.escapeSeqLen;
        isEscaped = true;
        break;
      }
    }
    // If character is not escaped
    if (!isEscaped)
    {
      // Increase result by 1
      length += 1;
    }
  }
  return length;
}

static void getEscapedString(char* result, const char* original,
                             size_t originalLen)
{
  char* resultTail = result;
  // For each original character
  for (size_t i = 0; i < originalLen; ++i)
  {
    bool isEscaped = false;
    // For each escape sequence
    for (const CharEscapeSeq& escape : ESCAPED_CHARS)
    {
      // If character needs to be escaped
      if (original[i] == escape.toEscape)
      {
        // Append escape sequence to to result
        memcpy(resultTail, escape.escapeSeq, escape.escapeSeqLen);
        resultTail += escape.escapeSeqLen;
        isEscaped = true;
        break;
      }
    }
    // If character is not escaped
    if (!isEscaped)
    {
      // Append original character to result
      *resultTail = original[i];
      resultTail += 1;
    }
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
    return "warning";
  case Severity::ERROR:
    return "error";
  case Severity::FATAL:
    return "fatal";
  default:
    return "unknown";
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

LogWriter::Status HtmlLogWriter::writeMessage(const LogMessage& message)
{
  // Check file descriptor validity
  assert(isValid && "Attempted write to invalid log file");
  // Check content type
  assert(message.contentType == LogMessage::ContentType::TEXT ||
         message.contentType == LogMessage::ContentType::CODE ||
         message.contentType == LogMessage::ContentType::IMAGE);

  const char* timestamp = getTimeString(message.timestamp);
  const char* severity  = getSeverityString(message.severity);
  // Write message header
  dprintf(logFd,
          "<p class=\"message\">"                 // Message start
          "<span class=\"timestamp\">%s</span>"   // Timestamp
          "<span class=\"severity %s\">%s</span>" // Severity
          "<span class=\"source\">'%s' in '%s' at '%s:%zu'</span>", // Source
          timestamp, severity, severity, message.source.logger,
          message.source.function, message.source.file, message.source.line);

  // If message content is image
  if (message.contentType == MessageContentType::IMAGE)
  {
    // Enclose content in <img\> tag
    dprintf(logFd, "<img src=\"%.*s\"/>", (int)message.contentLen,
            message.content);
  }
  else
  {
    const char* content          = message.content;
    size_t      contentLen       = message.contentLen;
    bool        isContentEscaped = needEscape(content, contentLen);

    // If message content must be escaped
    if (isContentEscaped)
    {
      // Generate escaped content
      size_t escapedLen     = getEscapedLen(content, contentLen);
      char*  escapedContent = new char[escapedLen];
      getEscapedString(escapedContent, content, contentLen);
      // Replace original content with escaped
      content    = escapedContent;
      contentLen = escapedLen;
    }

    // If message content is code
    if (message.contentType == MessageContentType::CODE)
    {
      // Write content in <code> tag
      dprintf(logFd, "<code\n>%.*s</code>", (int)contentLen, content);
    }
    else
    {
      // Write content in <span class="text"> tag
      dprintf(logFd, "<span class=\"text\">%.*s</span>", (int)contentLen,
              content);
    }

    if (isContentEscaped)
      delete[] content;
  }

  // Close message tag
  dprintf(logFd, "</p>\n");
  return LogWriter::Status::OK;
}

} // namespace meerkat_logs

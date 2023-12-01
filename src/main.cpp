#include <csignal>
#include <cstdio>
#include <cstdlib>

#include "mklog/LogManager.h"
#include "mklog/LogMessage.h"
#include "mklog/LogRoute.h"
#include "mklog/LogRoutingRule.h"
#include "mklog/Logger.h"
#include "mklog/writers/HtmlLogWriter.h"
#include "mklog/writers/StderrLogWriter.h"
#include "mklog/writers/TextLogWriter.h"

#define fassert(fd, condition, message)                                        \
  do                                                                           \
  {                                                                            \
    if (!(condition))                                                          \
    {                                                                          \
      dprintf(fd, "Failed assertion '%s': %s\n\tat %s:%d in function '%s'\n",  \
              #condition, message, __FILE__, __LINE__, __PRETTY_FUNCTION__);   \
      abort();                                                                 \
    }                                                                          \
  } while (0)

void dummyHandler(int) { puts("Interrupt handled in main.cpp"); }

void setupLogs()
{
  using mklog::LogManager; // TODO: Load config from file
  using mklog::LogRoute;
  using mklog::MessageSeverity;

  LogRoute minSeverityInfo =
      LogRoute::makeRoute<mklog::SeverityRoutingRule>(
          /* minSeverity = */ MessageSeverity::INFO);

  LogManager::addWriter<mklog::StderrLogWriter>()
      .useAnsiColors()
      .setRoute(minSeverityInfo);

  auto& textLogs = LogManager::addWriter<mklog::TextLogWriter>().setFile(
      ".log/log.txt");
  if (!textLogs.valid())
  {
    textLogs.setFile("log.txt");
  }

  auto& htmlLogs = LogManager::addWriter<mklog::HtmlLogWriter>().setFile(
      ".log/log.html");
  if (!htmlLogs.valid())
  {
    htmlLogs.setFile("log.html");
  }

  LogManager::initLogs();
}

int main()
{
  using mklog::Logger;
  using mklog::MessageContentType;
  using LogMessageFd = mklog::LogManager::MessageFd;

  sigset(SIGINT, &dummyHandler);
  setupLogs();

  Logger logger("main");

  logger.LOG_INFO(MessageContentType::TEXT, "Started logs");
  logger.LOG_TRACE(MessageContentType::TEXT, "Entered main()");
  logger.LOG_DEBUG(
      MessageContentType::CODE,
      "<span class=\"message\"> this is not message &amp; </span>\n"
      "<img src=\"img/image.png\"/>");
  logger.LOG_WARNING(MessageContentType::IMAGE, "img/log.png");
  LogMessageFd longMsgFd =
      logger.LOG_BEGIN_INFO(MessageContentType::TEXT, "This is a long message");

  dprintf(longMsgFd, "This is some content\n");

  raise(SIGINT);

  dprintf(longMsgFd, "This is some more content\n");
  dprintf(longMsgFd, "Long message ends here\n");

  logger.endLongMessage(longMsgFd);
  LogMessageFd emptyMsgFd =
      logger.LOG_BEGIN_FATAL(MessageContentType::TEXT, "This won't be printed");

  fassert(emptyMsgFd, 2 + 2 == 4, "This is clearly true");

  raise(SIGINT);

  fassert(emptyMsgFd, 2 * 2 == 4, "This is also true");

  logger.endLongMessage(emptyMsgFd);

  LogMessageFd assertFd =
      logger.LOG_BEGIN_FATAL(MessageContentType::TEXT, "VALIDATION ERROR");

  fassert(assertFd, 1 < 2, "True assertion");
  fassert(assertFd, 2 < 1, "False assertion");

  logger.endLongMessage(assertFd);

  return 0;
}

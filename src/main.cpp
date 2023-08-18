#include <csignal>
#include <cstdio>
#include <cstdlib>

#include "meerkat_logs/LogManager.h"
#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogRoute.h"
#include "meerkat_logs/LogRoutingRule.h"
#include "meerkat_logs/Logger.h"
#include "meerkat_logs/writers/StderrLogWriter.h"

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
  using meerkat_logs::LogManager;
  using meerkat_logs::LogRoute;
  using meerkat_logs::MessageSeverity;

  LogRoute minSeverityInfo =
      LogRoute::makeRoute<meerkat_logs::SeverityRoutingRule>(
          /* minSeverity = */ MessageSeverity::INFO);

  LogManager::addWriter<meerkat_logs::StderrLogWriter>()
      .useAnsiColors()
      .setRoute(minSeverityInfo);

  LogManager::initLogs();
}

int main()
{
  using meerkat_logs::Logger;
  using meerkat_logs::MessageContentType;
  using LogMessageFd = meerkat_logs::LogManager::MessageFd;

  sigset(SIGINT, &dummyHandler);
  setupLogs();

  Logger logger("main");

  logger.LOG_INFO(MessageContentType::TEXT, "Started logs");
  logger.LOG_TRACE(MessageContentType::TEXT, "Entered main()");
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

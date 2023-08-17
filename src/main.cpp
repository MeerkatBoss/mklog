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
      dprintf(fd, "Failed assertion '%s': %s\n\tat %s:%z in function '%s'\n",  \
              #condition, message, __FILE__, __LINE__, __PRETTY_FUNCTION__);   \
  } while (0)

void dummyHandler(int) { puts("Interrupt handled in main.cpp"); }

int main()
{
  using meerkat_logs::Logger;
  using meerkat_logs::LogManager;
  using meerkat_logs::LogRoute;
  using meerkat_logs::MessageContentType;
  using meerkat_logs::MessageSeverity;

  sigset(SIGINT, &dummyHandler);
  LogManager::addWriter<meerkat_logs::StderrLogWriter>(
      LogRoute::makeRoute<meerkat_logs::SeverityRoutingRule>(
          MessageSeverity::MIN_LEVEL),
      /* useEscapeCodes = */ true);
  LogManager::initLogs();
  Logger logger("main");

  logger.LOG_INFO(MessageContentType::TEXT, "Started logs");

  raise(SIGINT);
  raise(SIGINT);

  return 0;
}

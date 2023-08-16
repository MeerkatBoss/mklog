#include "meerkat_logs/LogManager.h"

#include <cassert>
#include <cstdlib>

#include "meerkat_logs/LogWriter.h"
#include "meerkat_logs/utils/SimpleList.h"

namespace meerkat_logs
{

utils::SimpleList<LogWriter*> LogManager::s_writerList =
    utils::SimpleList<LogWriter*>();

LogManager::Status LogManager::s_currentStatus =
    LogManager::Status::UNINITIALIZED;

void LogManager::endLogs()
{
  // Check that logs are started
  assert(s_currentStatus == Status::READY &&
         "Cannot end logs: logs not started");

  // For all writers
  for (LogWriter* writer : s_writerList)
  {
    // Delete writer
    delete writer;
  }
  // Clear list of writers
  s_writerList.clear();

  // Mark LogManager as deinitialized
  s_currentStatus = Status::UNINITIALIZED;
}

void LogManager::initLogs()
{
  // Check that logs are not started
  assert(s_currentStatus == Status::UNINITIALIZED &&
         "Cannot start logs: logs already started");

  // Register exit callbacks
  atexit(&LogManager::endLogs);

  // Mark LogManager as ready
  s_currentStatus = Status::READY;
}

void LogManager::logMessage(const LogMessage& message)
{
  // Check that LogManager is ready
  assert(s_currentStatus == Status::READY &&
         "Cannot send log message: LogManager is not ready");

  // For each registered writer
  for (LogWriter* writer : s_writerList)
  {
    // Try to send message to writer
    writer->tryWriteMessage(message);
  }
}

} // namespace meerkat_logs

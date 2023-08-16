/**
 * @file LogManager.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Main class for logging
 *
 * @version 0.1
 * @date 2023-08-13
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGMANAGER_H
#define __MEERKAT_LOGS_LOGMANAGER_H

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogWriter.h"

namespace meerkat_logs
{

/**
 * @brief Main class used for logging. Manages all LogWriters. Accepts and
 * distributes log messages.
 */
class LogManager
{
private:
  /**
   * @brief Linked list node containing LogWriter
   */
  struct WriterListNode
  {
    LogWriter*      writer;
    WriterListNode* next;
  };

  /**
   * @brief Head of linked list of LogWriter
   */
  static WriterListNode* s_writerListHead;

  /**
   * @brief State of LogManager
   */
  enum class Status
  {
    UNINITIALIZED,
    READY,
    DEINITIALIZED,
  };

  /**
   * @brief Current state of LogManager
   */
  static Status s_currentStatus;

  /**
   * @brief End all logging. Invalidate LogManager
   */
  static void endLogs();

public:
  /**
   * @brief Register new LogWriter in LogManager
   *
   * @tparam TWriter  LogWriter implementation
   *
   * @param[in] ...	  TWriter constructor arguments
   */
  template <typename TWriter, typename... TArgs>
  static void addWriter(TArgs... args)
  {
    s_writerListHead = new WriterListNode{.writer = new TWriter(args...),
                                          .next   = s_writerListHead};
  }

  /**
   * @brief Initialize logging for program
   */
  static void initLogs();

  /**
   * @brief Send log message to all registered writers
   *
   * @param[in] message	  Log message to be sent
   */
  static void logMessage(const LogMessage& message);
};

} // namespace meerkat_logs

#endif /* LogManager.h */

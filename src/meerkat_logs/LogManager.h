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
#include "meerkat_logs/utils/SimpleList.h"

namespace meerkat_logs
{

/**
 * @brief Main class used for logging. Manages all LogWriters. Accepts and
 * distributes log messages.
 */
class LogManager
{
public:
  /**
   * @brief File descriptor for long message content
   */
  using MessageFd = int;

  /**
   * @brief Maximum number of characters to append to long message content
   */
  static constexpr size_t LONG_MESSAGE_LEN_MAX = 4096;

private:
  static utils::SimpleList<LogWriter*> s_writerList;

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
  // Forbid construction of static class
  LogManager() = delete;

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
    s_writerList.pushFront(new TWriter(args...));
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

  /**
   * @brief Create file descriptor for new long message. Anything written
   * to returned `MessageFd` will be appended to `messageTemplate.content`.
   * If nothing is written to returned `MessageFd`, no message will be
   * written. Message is not guaranteed to be printed all at once, it may be
   * split into several messages. Total length of appended content for long
   * message may not exceed `LogManager::LONG_MESSAGE_LEN_MAX`.
   *
   * @param[in] messageTemplate	  Template for long message
   *
   * @return File descriptor for message content
   *
   * @warning  Any attempt to write more than `LogMessage::LONG_MESSAGE_LEN_MAX`
   * to returned `MessageFd` results in undefined behaviour.
   */
  static MessageFd beginLongMessage(const LogMessage& messageTemplate);

  /**
   * @brief End long message with file descriptor `fd`. If any content has
   * been appended, the message is guaranteed to be printed after call to this
   * function.
   *
   * @param[inout] fd     Long message file descriptor
   *
   * @warning `fd` is invalidated after call to this function. Do not use it
   * after call
   */
  static void endLongMessage(MessageFd& fd);
};

} // namespace meerkat_logs

#endif /* LogManager.h */

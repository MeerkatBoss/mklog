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

class LogManager
{
public:
  static void logMessage(const LogMessage& message);
};

} // namespace meerkat_logs

#endif /* LogManager.h */

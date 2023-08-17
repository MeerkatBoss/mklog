/**
 * @file LogRoutingRule.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Rules for routing LogMessages
 *
 * @version 0.1
 * @date 2023-08-13
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGROUTINGRULE_H
#define __MEERKAT_LOGS_LOGROUTINGRULE_H

#include "meerkat_logs/LogMessage.h"

namespace meerkat_logs
{

/**
 * @brief Rule for routing LogMessages
 */
class LogRoutingRule
{
public:
  /**
   * @brief Check if message matches this routing rule
   *
   * @param[in] message   Message to be matched
   *
   * @return `true` if message matches routing rule, `false` otherwise
   */
  virtual bool matchMessage(const LogMessage& message) const = 0;

  virtual ~LogRoutingRule() = default;
};

class SeverityRoutingRule : public LogRoutingRule
{
private:
  LogMessage::Severity minSeverity;

public:
  SeverityRoutingRule(LogMessage::Severity minSeverity)
      : minSeverity(minSeverity)
  {
  }

  bool matchMessage(const LogMessage& message) const override
  {
    return message.severity >= minSeverity;
  }
};

} // namespace meerkat_logs

#endif /* LogRoutingRule.h */

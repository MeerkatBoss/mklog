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

#include "mklog/LogMessage.h"

namespace mklog
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

/**
 * @brief Route all messages
 */
class DefaultRoutingRule : public LogRoutingRule
{
  bool matchMessage(const LogMessage&) const override { return true; }
};

} // namespace mklog

#endif /* LogRoutingRule.h */

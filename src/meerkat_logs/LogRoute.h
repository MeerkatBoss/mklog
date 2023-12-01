/**
 * @file LogRoute.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Wrapper for LogRoutingRule to avoid dangling references
 *
 * @version 0.1
 * @date 2023-08-12
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_LOGROUTE_H
#define __MEERKAT_LOGS_LOGROUTE_H

#include "meerkat_logs/LogMessage.h"
#include "meerkat_logs/LogRoutingRule.h"

namespace meerkat_logs
{

/**
 * @brief Filter for log messages
 *
 */
class LogRoute
{
private: // TODO: Maybe write public before private
  /// Rule controlling the filter
  LogRoutingRule* routingRule;

  /// Number of LogRoutes referencing routingRule
  size_t* refCount;

  LogRoute(LogRoutingRule* routingRule, size_t* refCount)
      : routingRule(routingRule), refCount(refCount) // refCount(new size_t{1})
  {
  }

public:
  /**
   * @brief Create new LogRoute with routing rule specified as TRule
   *
   * @tparam TRule  routing rule
   *
   * @param[in] args	Constructor arguments for TRule
   *
   * @return Constructed LogRoute instance
   */
  template <typename TRule, typename... TArgs>
  static LogRoute makeRoute(TArgs... args)
  {
    return LogRoute(new TRule(args...), new size_t{1});
  }

  LogRoute(const LogRoute& other)
      : routingRule(other.routingRule), refCount(other.refCount)
  {
    *refCount += 1;
  }

  LogRoute& operator=(const LogRoute& other)
  {
    if (this == &other)
      return *this;

    this->~LogRoute(); // TODO: Extract to function, do not call dtor

    routingRule = other.routingRule;
    refCount    = other.refCount;
    *refCount += 1;

    return *this;
  }

  /**
   * @brief Check if message matches routing rule of this route
   *
   * @param[in] message	  Message to be matched
   *
   * @return `true` if message matches routing rule, `false` otherwise
   */
  bool matchMessage(const LogMessage& message) const
  {
    return routingRule->matchMessage(message);
  }

  ~LogRoute()
  {
    *refCount -= 1;
    if (*refCount == 0)
    {
      delete routingRule;
      delete refCount;
    }
  }
};

/**
 * @brief Logical AND for routes
 */
class LogRoutingRuleAnd : public LogRoutingRule
{
private:
  LogRoute firstRoute;
  LogRoute secondRoute;

public:
  LogRoutingRuleAnd(const LogRoute& first, const LogRoute& second)
      : firstRoute(first), secondRoute(second)
  {
  }

  virtual bool matchMessage(const LogMessage& message) const override
  {
    return firstRoute.matchMessage(message) &&
           secondRoute.matchMessage(message);
  }
};

/**
 * @brief Logical OR for routes
 */
class LogRoutingRuleOr : public LogRoutingRule
{
private:
  LogRoute firstRoute;
  LogRoute secondRoute;

public:
  LogRoutingRuleOr(const LogRoute& first, const LogRoute& second)
      : firstRoute(first), secondRoute(second)
  {
  }

  virtual bool matchMessage(const LogMessage& message) const override
  {
    return firstRoute.matchMessage(message) ||
           secondRoute.matchMessage(message);
  }
};

/**
 * @brief Logical NOT for routes
 */
class LogRoutingRuleNot : public LogRoutingRule
{
private:
  LogRoute route;

public:
  LogRoutingRuleNot(const LogRoute& route) : route(route) {}

  virtual bool matchMessage(const LogMessage& message) const override
  {
    return !route.matchMessage(message);
  }
};

inline LogRoute operator&&(const LogRoute& first, const LogRoute& second)
{
  return LogRoute::makeRoute<LogRoutingRuleAnd>(first, second);
}

inline LogRoute operator||(const LogRoute& first, const LogRoute& second)
{
  return LogRoute::makeRoute<LogRoutingRuleOr>(first, second);
}

inline LogRoute operator!(const LogRoute& route)
{
  return LogRoute::makeRoute<LogRoutingRuleNot>(route);
}

} // namespace meerkat_logs

#endif /* LogRoute.h */

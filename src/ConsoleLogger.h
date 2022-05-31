#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "xnLogger.h"

class ConsoleLogger : public xn::Logger
{
public:

  ConsoleLogger();
  ~ConsoleLogger();

  bool Init() override;
  void Destroy() override;

  void LogDebug(char const *) override;
  void LogInfo(char const *) override;
  void LogWarning(char const *) override;
  void LogError(char const *) override;
};

#endif
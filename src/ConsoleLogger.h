#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "a2dLogger.h"

class ConsoleLogger : public a2d::Logger
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
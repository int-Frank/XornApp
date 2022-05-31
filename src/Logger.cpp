#include "appLogger.h"

a2d::Logger *g_pLogger = nullptr;

void SetLogger(a2d::Logger *pLogger)
{
  DestroyLogger();
  g_pLogger = pLogger;
}

void DestroyLogger()
{
  delete g_pLogger;
  g_pLogger = nullptr;
}

a2d::Logger* GetLogger()
{
  return g_pLogger;
}
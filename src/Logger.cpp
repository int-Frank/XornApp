#include "Logger.h"

xn::Logger *g_pLogger = nullptr;

void SetLogger(xn::Logger *pLogger)
{
  DestroyLogger();
  g_pLogger = pLogger;
}

void DestroyLogger()
{
  delete g_pLogger;
  g_pLogger = nullptr;
}

xn::Logger* GetLogger()
{
  return g_pLogger;
}
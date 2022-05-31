
#include <stdio.h>

#include "appConsoleLogger.h"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET_COLOUR "\033[0m"

ConsoleLogger::ConsoleLogger()
{
}

ConsoleLogger::~ConsoleLogger()
{
}

bool ConsoleLogger::Init()
{
  return true;
}

void ConsoleLogger::Destroy()
{
}

void ConsoleLogger::LogDebug(char const *pMsg)
{
  printf(GREEN "DEBUG: %s\n" RESET_COLOUR, pMsg);
}

void ConsoleLogger::LogInfo(char const *pMsg)
{
  printf("%s\n", pMsg);
}

void ConsoleLogger::LogWarning(char const *pMsg)
{
  printf(YELLOW "WARNING: %s\n" RESET_COLOUR, pMsg);
}

void ConsoleLogger::LogError(char const *pMsg)
{
  printf(RED "ERROR: %s\n" RESET_COLOUR, pMsg);
}

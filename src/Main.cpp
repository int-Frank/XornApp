
#include "App.h"
#include "Logger.h"
#include "ConsoleLogger.h"

int main(int, char**)
{
  try
  {
    xn::Logger *pLogger = new ConsoleLogger();
    if (!pLogger->Init())
      throw std::exception("Failed to initialise the Logger!");
    SetLogger(pLogger);

    App *pApp = new App();
    pApp->Run();
    delete pApp;
  }
  catch (std::exception e)
  {
    LOG_ERROR("App failed with exception: %s", e.what());
  }

  DestroyLogger();

  return 0;
}

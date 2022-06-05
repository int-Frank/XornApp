#ifndef APPLOGGER_H
#define APPLOGGER_H

#include "xnLogger.h"
#include "xnCommon.h"

void SetLogger(xn::Logger *);
void DestroyLogger();
xn::Logger *GetLogger();

// Application should use these
#define LOG_DEBUG(...)   do{if (GetLogger() != nullptr) {char *__LOGMSGBUF = nullptr; ::xn::asprintf(&__LOGMSGBUF, __VA_ARGS__); GetLogger()->LogDebug(__LOGMSGBUF); delete[] __LOGMSGBUF;}} while(false)
#define LOG_INFO(...)   do{if (GetLogger() != nullptr) {char *__LOGMSGBUF = nullptr; ::xn::asprintf(&__LOGMSGBUF, __VA_ARGS__); GetLogger()->LogInfo(__LOGMSGBUF); delete[] __LOGMSGBUF;}} while(false)
#define LOG_WARNING(...)   do{if (GetLogger() != nullptr) {char *__LOGMSGBUF = nullptr; ::xn::asprintf(&__LOGMSGBUF, __VA_ARGS__); GetLogger()->LogWarning(__LOGMSGBUF); delete[] __LOGMSGBUF;}} while(false)
#define LOG_ERROR(...)   do{if (GetLogger() != nullptr) {char *__LOGMSGBUF = nullptr; ::xn::asprintf(&__LOGMSGBUF, __VA_ARGS__); GetLogger()->LogError(__LOGMSGBUF); delete[] __LOGMSGBUF;}} while(false)

#endif
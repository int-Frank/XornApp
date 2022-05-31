#ifndef APPLOGGER_H
#define APPLOGGER_H

#include "xnLogger.h"
#include "xnCommon.h"

void SetLogger(xn::Logger *);
void DestroyLogger();
xn::Logger *GetLogger();

// Application should use these
#define LOG_DEBUG(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::xn::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogDebug(pMsg); delete[] pMsg;}} while(false)
#define LOG_INFO(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::xn::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogInfo(pMsg); delete[] pMsg;}} while(false)
#define LOG_WARNING(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::xn::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogWarning(pMsg); delete[] pMsg;}} while(false)
#define LOG_ERROR(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::xn::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogError(pMsg); delete[] pMsg;}} while(false)

#endif
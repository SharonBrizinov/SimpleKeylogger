# Simple Keylogger

Simple keylogger using Windows hooks mechanisem. Features:
* Supports foreign languages
* Keys are stored in a log file ("encrypted" using XOR)
* Suicide mechanisem
* Persistency using `CurrentVersion\Run` registry key
* Single instance running using Mutex
* Detailed configuration at compile time

# Hooks
* [WH_KEYBOARD_LL](https://learn.microsoft.com/en-us/windows/win32/winmsg/about-hooks#wh_keyboard_ll) - The WH_KEYBOARD_LL hook enables you to monitor keyboard input events about to be posted in a thread input queue.


I wrote this tool as a learning project to understand some of the inner working of Windows better. 

For educational purposes only, please use responsibly.

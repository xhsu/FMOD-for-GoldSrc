/*

Created Date: Apr 07 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define charsmax(x)		(sizeof(x) - 1)
#define wcharsmax(x)	(sizeof(x) / sizeof(wchar_t) - 1)

inline void NORETURN Sys_Error(const char* fmt, ...)
{
	static char output[1024];
	va_list ptr;

	va_start(ptr, fmt);
	vsnprintf(output, charsmax(output), fmt, ptr);
	va_end(ptr);

	HWND hwnd = GetActiveWindow();
	MessageBoxA(hwnd, output, "Fatal Error", MB_OK | MB_ICONERROR | MB_TOPMOST);

	_exit(-1);
}

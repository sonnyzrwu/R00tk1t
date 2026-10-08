#include <windows.h>
#include <stdio.h>


int (*originalMsgBox)(HWND, LPCSTR, LPCSTR, UINT) = MessageBoxA;

int MyEvilFunction(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    printf("This is my evil function!\n");
    return originalMsgBox(hWnd, lpText, lpCaption, uType);
}


void ParsePE(LPCSTR functionToHook ){


    PVOID imageBase = GetModuleHandle(NULL);
    PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER) imageBase;
    PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)((BYTE*)imageBase + dosHeaders->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
    SIZE_T imageSize = ntHeaders->OptionalHeader.SizeOfImage;
    IMAGE_DATA_DIRECTORY importDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importDirectory.VirtualAddress + (DWORD_PTR)imageBase);

    while ( importDescriptor->Name != NULL){
        LPCSTR dllName = (LPCSTR) (importDescriptor->Name + (DWORD_PTR) imageBase);
        printf("DLL Name: %s\n", dllName);
        // OriginalFirstThunk (also called as Import Lookup Table) contains the supposed addresses of the imported functions.
        PIMAGE_THUNK_DATA originalFirstThunk = (PIMAGE_THUNK_DATA)(importDescriptor->OriginalFirstThunk + (DWORD_PTR)imageBase);
        /*
        The structure and content of the Import Address Table are identical to that of the 
        Import Lookup Table, until the file is bound. During binding, the entries in the 
        Import Address Table are overwritten with the 32-bit (or 64-bit for PE32+) addresses
        of the symbols being imported: these addresses are the actual memory addresses of the 
        symbols themselves (although technically, they are still called “virtual addresses”). 
        The processing of binding is typically performed by the loader.
        */
        PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA) (importDescriptor->FirstThunk  + (DWORD_PTR) imageBase);

        while (originalFirstThunk->u1.AddressOfData != NULL){
            PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(originalFirstThunk->u1.AddressOfData + (DWORD_PTR)imageBase);
            LPCSTR functionName = (LPCSTR)importByName->Name;
            printf("Function Name: %s\n", functionName);

            if (strcmp(functionName, functionToHook) == 0){
                printf("Found %s function at address: %p\n", functionToHook, firstThunk->u1.Function);
                DWORD oldProtect = 0;
                VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, PAGE_EXECUTE_READWRITE, &oldProtect);
                // Overwrite the function pointer in the IAT with the address of our custom function
                firstThunk->u1.Function = (DWORD_PTR)MyEvilFunction;    
                return;
            }
            
            originalFirstThunk++;
            firstThunk++;
        }
        importDescriptor++;
    }
    printf("Could not find the function %s in the IAT.\n", functionToHook);
}


BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    printf("DLLMain called!\n");    
    ParsePE("MessageBoxA");
    // Find out how to continue execution of the victim process after the DLL is injected.
    return TRUE;
}




    



// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <vector>
#include <fstream>

typedef unsigned char byte;
bool unprotect(int* ptr, int len, PDWORD oldp) {
    return VirtualProtect((LPVOID)(ptr), len, PAGE_EXECUTE_READWRITE, oldp);
}
bool protect(int* ptr, int len, PDWORD oldp) {
    DWORD dummy;
    return VirtualProtect((LPVOID)(ptr), len, *oldp, &dummy);
}

bool apply(int* ptr, int replace) {
    DWORD protection;
    if (!unprotect(ptr, sizeof(replace), &protection)) {
        return false;
    }
    if (!memcpy(ptr, &replace, sizeof(replace))) {
        return false;
    }
    if (!protect(ptr, sizeof(replace), &protection)) {
        return false;
    }
    return true;
}

void ApplyChanges() {

    auto module = GetModuleHandleA("Nights.exe");
    if (module == nullptr) {
        return;
    }

    byte* startAdd = (byte*)module;
    byte* ccTable = startAdd + 0x3648b8;
    int* AllAccess = (int*)ccTable;
    int* ClarisCostume = (int*)(ccTable - 0x14C);

    apply(&AllAccess[3], 4096); //change Cross/A to Triangle/Y
    apply(&ClarisCostume[3], 32768); //change Cross/A to Square/X
    /*
    std::ofstream log("addressLog.txt");
    log << "All Access: " << AllAccess << std::endl;
    log << "Claris Costumes: " << ClarisCostume << std::endl;
    log.close();
    //*/
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ApplyChanges();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


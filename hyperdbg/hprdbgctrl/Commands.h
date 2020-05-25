/**
 * @file commands.h
 * @author Sina Karvandi (sina@rayanfam.com)
 * @brief The hyperdbg command interpreter and driver connector
 * @details
 * @version 0.1
 * @date 2020-04-11
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#pragma once
#include "pch.h"

using namespace std;

extern HANDLE DeviceHandle;

int ReadCpuDetails();
std::string ReadVendorString();
void ShowMessages(const char *Fmt, ...);
int CommandLm(vector<string> SplittedCommand);


void HyperDbgReadMemoryAndDisassemble(DEBUGGER_SHOW_MEMORY_STYLE Style,
                                      UINT64 Address,
                                      DEBUGGER_READ_MEMORY_TYPE MemoryType,
                                      DEBUGGER_READ_READING_TYPE ReadingType,
                                      UINT32 Pid, UINT Size);
string SeparateTo64BitValue(UINT64 Value);

int HyperDbgDisassembler(unsigned char *BufferToDisassemble, UINT64 BaseAddress,
                         UINT64 Size);

// Exports
extern "C" {
extern bool inline AsmVmxSupportDetection();
__declspec(dllexport) int __cdecl HyperdbgLoad();
__declspec(dllexport) int __cdecl HyperdbgUnload();
__declspec(dllexport) int __cdecl HyperdbgInstallDriver();
__declspec(dllexport) int __cdecl HyperdbgUninstallDriver();
__declspec(dllexport) void __stdcall HyperdbgSetTextMessageCallback(
    Callback handler);
}

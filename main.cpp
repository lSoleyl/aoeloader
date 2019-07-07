#include <Windows.h>

#include <sstream>
#include <iostream>
#include <vector>


#include "nop_memory.h"
#include "win_error.h"



NOP_MEMORY buildLimit(0x008f601e, 25);
NOP_MEMORY populationCap(0x004278bc, 62);


const char* PROGRAM_FILE = "age3x.exe";

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int showCmd) {
  try {
    // Launch our process in suspended state
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcess("age3x.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
      HandleError("CreateProcess");
    }


    //TODO validate we have the correct version of this process


    // Patch the process memory
    if (!buildLimit.Apply(pi.hProcess)) {
      HandleError("WriteProcessMemory(BuildLimit)");
    }

#ifndef KEEP_POPULATION_CAP
    if (!populationCap.Apply(pi.hProcess)) {
      HandleError("WriteProcessMemory(PopulationCap)");
    }
#endif

    // Now let the process run
    if (ResumeThread(pi.hThread) == -1) {
      HandleError("ResumeThread");
    }

  } catch (std::exception& ex) {
    MessageBox(NULL, ex.what(), "AOE loader failed", MB_ICONERROR);
  }
}

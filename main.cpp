#include <Windows.h>

#include <sstream>
#include <iostream>
#include <vector>

#include "win_error.h"
#include "patch_memory.h"



// This memory address initializes some kind of getter for the BuildLimit property skipping this code
// removes the build limit for most units
NOP_MEMORY buildLimit(0x008f601e, 25, "8d442408 5068e069 b5008bce c7442410"
                                      "76098f00 e8e858b7 ff");

// For Banks, Forts, ... (units with variable build limit?) the Build limit gets set to 1 instead and the following patch
// will explicitly return -1 (infinite) for these units
WRITE_MEMORY buildLimitAccess(0x00453ca0, "b8 ffffffff 90", "8b800801 0000"); // b8 ffffffff 90 == mov eax, -1; nop...

// This skips the complete population cap check.
NOP_MEMORY populationCap(0x004278bc, 62, "3dc80000 000f8f9c 1008008b 0da0e4bc"
                                         "008b893c 0100008b 891c0300 003bc10f"
                                         "8f8c1008 008b92b4 04000085 d20f8f85"
                                         "1008003d fa000000 0f8f8110 0800");


const char* PROGRAM_FILE = "age3x.exe";

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int showCmd) {
  try {
    // Launch our process in suspended state
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcess(PROGRAM_FILE, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
      HandleError("CreateProcess");
    }


    // Make sure we are trying to launch the supported version of the game
    if (!buildLimit.Check(pi.hProcess) || !buildLimitAccess.Check(pi.hProcess) || !populationCap.Check(pi.hProcess)) {
      TerminateProcess(pi.hProcess, 0);
      throw std::exception("The process game version doesn't match the only supported version. Launch aborted.");
    }


    // Patch the process memory
    if (!buildLimit.Apply(pi.hProcess)) {
      HandleError("WriteProcessMemory(BuildLimit)");
    }


    if (!buildLimitAccess.Apply(pi.hProcess)) {
      HandleError("WriteProcessMemory(BuildLimitAccess)");
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

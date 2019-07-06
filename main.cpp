#include <Windows.h>

#include <sstream>
#include <iostream>
#include <vector>

/** Just launch the age3x.exe from the current working directory
 */



void HandleError(const char* context) {
  char* buffer;
  auto err = GetLastError();
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, NULL, (LPSTR)&buffer, NULL, NULL);

  std::ostringstream msg;
  msg << context << ": " << buffer;
  LocalFree(buffer);
  throw std::exception(msg.str().c_str());
}


void* TARGET_ADDRESS = reinterpret_cast<void*>(0x008f601e);

const uint8_t NOP = 0x90;

struct NOP_MEMORY {
  NOP_MEMORY(uint32_t addr, size_t size) : startAddress(reinterpret_cast<void*>(addr)), bytes(size, NOP) {}

  BOOL Apply(HANDLE hProcess) {
    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(hProcess, startAddress, bytes.data(), bytes.size(), &bytesWritten);
  }

  void* startAddress;
  std::vector<uint8_t> bytes;
};



NOP_MEMORY buildLimit(0x008f601e, 25);
NOP_MEMORY populationCap(0x004278bc, 0x004278fa-0x004278bc);


 
// We must patch the memory range 0x008f601e - 0x008f6037(exclusive) with NOPs (0x90)

const char* PROGRAM_FILE = "age3x.exe";

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int showCmd) {
  try {
    //TODO check whether file exists
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcess("age3x.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
      HandleError("CreateProcess()");
    }

    //TODO validate we have the correct version of this process


    if (!buildLimit.Apply(pi.hProcess)) {
      HandleError("WriteProcessMemory(BuildLimit)");
    }

    if (!populationCap.Apply(pi.hProcess)) {
      HandleError("WriteProcessMemory(PopulationCap)");
    }

    // Now let the process run
    if (ResumeThread(pi.hThread) == -1) {
      HandleError("ResumeThread()");
    }

  } catch (std::exception& ex) {
    MessageBox(NULL, ex.what(), "AOE loader failed", MB_ICONERROR);
  }
}

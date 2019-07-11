#pragma once

#include "hexstring.h"

struct WRITE_MEMORY {
  WRITE_MEMORY(uint32_t addr, const char* bytes, const char* originalBytes) : startAddress(reinterpret_cast<void*>(addr)), bytes(hexstring(bytes)), originalBytes(hexstring(originalBytes)) {}
  WRITE_MEMORY(uint32_t addr, std::initializer_list<uint8_t> bytes, const char* originalBytes) : startAddress(reinterpret_cast<void*>(addr)), bytes(bytes), originalBytes(hexstring(originalBytes)) {}
  WRITE_MEMORY(uint32_t addr, std::vector<uint8_t>&& bytes, const char* originalBytes) : startAddress(reinterpret_cast<void*>(addr)), bytes(std::move(bytes)), originalBytes(hexstring(originalBytes)) {}


  bool Apply(HANDLE hProcess) const {
    if (bytes.empty()) {
      throw std::exception("No data to patch");
    }
    SIZE_T bytesWritten = 0;
    return (WriteProcessMemory(hProcess, startAddress, bytes.data(), bytes.size(), &bytesWritten) == TRUE);
  }

  /** Validate that the process memory contains the expected data and the patch can thus be applied.
   */
  bool Check(HANDLE hProcess) const {
    std::vector<uint8_t> processMemory(originalBytes.size());
    SIZE_T bytesRead = 0;
    if (!ReadProcessMemory(hProcess, startAddress, processMemory.data(), processMemory.size(), &bytesRead)) {
      HandleError("ReadProcessMemory()");
    }

    // All bytes must match for the patch to safely work
    return (processMemory == originalBytes);
  }

private:
  void* startAddress;
  std::vector<uint8_t> bytes;
  std::vector<uint8_t> originalBytes; // This is used to verify the program version
};

const uint8_t NOP = 0x90;

struct NOP_MEMORY : WRITE_MEMORY {
  NOP_MEMORY(uint32_t addr, size_t size, const char* originalBytes) : WRITE_MEMORY(addr, std::vector<uint8_t>(size, NOP), originalBytes) {}  
};

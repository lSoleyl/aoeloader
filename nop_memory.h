#pragma once


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

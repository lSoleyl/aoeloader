#pragma once


struct WRITE_MEMORY {
  WRITE_MEMORY(uint32_t addr, std::initializer_list<uint8_t> bytes) : startAddress(reinterpret_cast<void*>(addr)), bytes(bytes) {}
  WRITE_MEMORY(uint32_t addr, std::vector<uint8_t>&& bytes) : startAddress(reinterpret_cast<void*>(addr)), bytes(std::move(bytes)) {}


  BOOL Apply(HANDLE hProcess) {
    if (bytes.empty()) {
      throw std::exception("No data to patch");
    }
    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(hProcess, startAddress, bytes.data(), bytes.size(), &bytesWritten);
  }

  void* startAddress;
  std::vector<uint8_t> bytes;
};

const uint8_t NOP = 0x90;

struct NOP_MEMORY : WRITE_MEMORY {
  NOP_MEMORY(uint32_t addr, size_t size) : WRITE_MEMORY(addr, std::vector<uint8_t>(size, NOP)) {}  
};

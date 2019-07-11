#pragma once

#include <vector>

/** Helper function convert a hex character into a decimal number
 */
uint8_t hexchar(char ch) {
  if ('0' <= ch && ch <= '9') {
    return ch - '0'; // decimal number
  }

  if (ch > 'F') {
    ch -= 0x20; // toupper for all lowercase letters
  }

  if ('A' <= ch && ch <= 'F') {
    return (ch - 'A') + 10;
  }

  return 0; // unknown
}

/** Converts the given string from hexadecimal into binary. The string may contain
 *  spaces between bytes, to format it visually.
 */
std::vector<uint8_t> hexstring(const char* string) {
  std::vector<uint8_t> binary;

  for (auto pos = string; *pos; pos+=2) {
    while (*pos == ' ') {
      ++pos; // skip all spaces to support formatting
    }
    binary.push_back((hexchar(pos[0]) << 4) | hexchar(pos[1]));
  }

  return binary;
};

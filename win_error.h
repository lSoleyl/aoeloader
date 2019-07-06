#pragma once

/** Generic windows error handler function (throws the error as std::exception)
*/
void HandleError(const char* context) {
  char* buffer;
  auto err = GetLastError();
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, NULL, (LPSTR)&buffer, NULL, NULL);

  std::ostringstream msg;
  msg << context << ": " << buffer;
  LocalFree(buffer);
  throw std::exception(msg.str().c_str());
}

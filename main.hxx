// requires
// #include <filesystem>
// #include <sstream>
//
// #include <windows.h>
// #include <tchar.h>

HANDLE stdio_out{GetStdHandle(STD_OUTPUT_HANDLE)};
HANDLE stdio_err{GetStdHandle(STD_ERROR_HANDLE)};
void writeGeneric(std::wstring const &&ws, HANDLE const &stream) {
  int csize{WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, NULL, 0, NULL, NULL)};
  char *cstr = new char[csize]{};
  WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, cstr, csize, NULL, NULL);
  DWORD bytes_written{0};
  WriteFile(stream, cstr, csize - 1, &bytes_written, NULL);
  delete[] cstr;
}
void writeOut(std::wstringstream &wss) {
  wss << "\n";
  writeGeneric(wss.str(), stdio_out);
}
void writeOut(std::wstring &&ws) {
  ws += L'\n';
  writeGeneric(std::move(ws), stdio_out);
}
void writeErr(std::wstringstream &wss) {
  wss << "\n";
  writeGeneric(wss.str(), stdio_err);
}
void writeErr(std::wstring &&ws) {
  ws += L'\n';
  writeGeneric(std::move(ws), stdio_err);
}

std::filesystem::path resolvePath(std::filesystem::path const &path) {
  // `requiredSize` includes space for the terminating null character
  DWORD requiredSize = GetFullPathNameW(path.c_str(), 0, NULL, NULL);
  if (requiredSize > 0) {
    TCHAR *lpBuffer = new TCHAR[requiredSize]{};
    // `copiedSize` does NOT include the terminating null character
    DWORD copiedSize = GetFullPathNameW(path.c_str(), requiredSize, lpBuffer, NULL);
    // remove trailing slash
    switch (lpBuffer[copiedSize - 1]) {
      case '\\':
      case '/':
        lpBuffer[copiedSize - 1] = 0;
        break;
    }
    std::filesystem::path resolvedPath{lpBuffer};
    delete[] lpBuffer;
    return resolvedPath;
  }
  return std::filesystem::path{""};
}
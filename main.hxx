// requires
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
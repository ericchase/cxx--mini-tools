#define UNICODE
#define _UNICODE

#include <filesystem>
#include <iostream>
#include <sstream>

#include <windows.h>
#include <tchar.h>

#include "help.hxx"
#include "../info.hxx"
#include "../main.hxx"

/**
 * Adapted from https://gist.github.com/nickav/a57009d4fcc3b527ed0f5c9cf30618f8
 * Assuming we are allowed to copy this as it was presented as example code.
 */
int listDirectory(TCHAR *path, TCHAR *filter = NULL) {
  std::filesystem::path resolved_path{resolvePath(path)};
  std::wstringstream find_path{};
  find_path << L"\\\\?\\" << resolved_path.c_str();
  if (filter == NULL) {
    find_path << L"\\*";
  } else {
    find_path << L"\\" << filter;
  }

  WIN32_FIND_DATAW find_data{};
  HANDLE hFind{FindFirstFileExW( //
      find_path.str().c_str(),   // file name can include wildcard characters
      FindExInfoBasic,           // does not query the short file name, improving overall enumeration speed
      &find_data,                // buffer that receives the file data
      FindExSearchNameMatch,     // search for a file that matches the specified file name
      NULL,                      // NULL when this search operation is used
      FIND_FIRST_EX_LARGE_FETCH  // uses a larger buffer for directory queries, which can increase performance of the
                                 // find operation
      )};
  if (hFind == INVALID_HANDLE_VALUE) {
    DWORD last_error{GetLastError()};
    switch (last_error) {
      case 2:
        writeErr(L"2 " + std::wstring{path});
        break;
      case 18:
        // There are no more files.
        break;
      default: {
        std::wstringstream _{};
        _ << "0 FindFirstFileExW GetLastError: " << last_error;
        writeErr(_);
        break;
      }
    }
    goto _Exit;
  }

_FindNext: {
  size_t name_length = wcslen(find_data.cFileName);
  if ((name_length == 1 && find_data.cFileName[0] == '.') ||
      (name_length == 2 && find_data.cFileName[0] == '.' && find_data.cFileName[1] == '.')) {
    // skip . and ..
  } else {
    // get attributes to determine if file or directory
    HANDLE hFile{CreateFileW(                                                     //
        std::filesystem::path{resolved_path}.append(find_data.cFileName).c_str(), //
        FILE_READ_ATTRIBUTES,                                                     // The right to read file attributes.
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,                   //
        NULL,                                                                     //
        OPEN_EXISTING,                                                            //
        FILE_FLAG_BACKUP_SEMANTICS,                                               //
        NULL                                                                      //
        )};
    std::wstring relative_path{std::wstring{path} + L"\\" + find_data.cFileName};
    if (hFile == INVALID_HANDLE_VALUE) {
      writeErr(L"1 " + relative_path);
    } else {
      BY_HANDLE_FILE_INFORMATION file_info{};
      BOOL bSuccess{GetFileInformationByHandle(hFile, &file_info)};
      std::wstring path_kind{(file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? L"D" : L"F"};
      writeOut(path_kind + L" " + find_data.cFileName);
    }
    CloseHandle(hFile);
  }

  // find next file
  BOOL bSuccess{FindNextFileW(hFind, &find_data)};
  if (bSuccess == 0) {
    DWORD last_error{GetLastError()};
    switch (last_error) {
      case 18:
        // There are no more files.
        break;
      default: {
        std::wstringstream _{};
        _ << "0 FindNextFileW GetLastError: " << last_error;
        writeErr(_);
        break;
      }
    }
    goto _Exit;
  }
  goto _FindNext;
}

_Exit:
  FindClose(hFind);
  return 0;
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  SetConsoleOutputCP(65001);          // set console output code page to utf-8

  switch (argc) { // 0th arg (argc=1) is program path
    case 2:
      return listDirectory(argv[1]);
    case 3:
      return listDirectory(argv[1], argv[2]);
      break;
    default: {
      std::wstringstream wss{};
      wss << Help << "\n\n" << Break << "\n\n" << Info;
      writeOut(wss);
      break;
    }
  }
  return 0;
}
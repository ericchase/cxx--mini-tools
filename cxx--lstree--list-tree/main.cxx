#define UNICODE
#define _UNICODE

#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

#include <windows.h>
#include <tchar.h>

#include "help.hxx"
#include "../info.hxx"
#include "../main.hxx"

struct Listing {
  boolean is_directory{false};
  boolean is_symlink{false};
  size_t prepend_index{0};
  std::wstring path{L""};
};

/**
 * Adapted from https://gist.github.com/nickav/a57009d4fcc3b527ed0f5c9cf30618f8
 * Assuming we are allowed to copy this as it was presented as example code.
 */
int listTree(TCHAR const *const path_arg) {
  // std::wstring root{resolvePath(path_arg)};
  std::vector<Listing> tree{};
  tree.push_back({true, false, 0, resolvePath(path_arg)});
  size_t last_count{1000};

  for (size_t index = 0; index < tree.size(); ++index) {
    if (!tree[index].is_directory || tree[index].is_symlink) {
      continue;
    }

    WIN32_FIND_DATAW find_data{};
    HANDLE hFind{FindFirstFileExW(                        //
        (L"\\\\?\\" + tree[index].path + L"\\*").c_str(), // file name can include wildcard characters
        FindExInfoBasic,          // does not query the short file name, improving overall enumeration speed
        &find_data,               // buffer that receives the file data
        FindExSearchNameMatch,    // search for a file that matches the specified file name
        NULL,                     // NULL when this search operation is used
        FIND_FIRST_EX_LARGE_FETCH // uses a larger buffer for directory queries, which can increase performance of the
                                  // find operation
        )};
    if (hFind == INVALID_HANDLE_VALUE) {
      DWORD last_error{GetLastError()};
      switch (last_error) {
        case 2:
          writeErr(L"2 FindFirstFile. No files found.");
          break;
        case 18:
          // There are no more files.
          break;
        default: {
          // std::wstringstream _{};
          // _ << "0 FindFirstFileExW GetLastError: " << last_error;
          // writeErr(_);
          break;
        }
      }
      // goto _FindClose;
      continue;
    }

_FindNext:
    // block
    {
      size_t name_length = wcslen(find_data.cFileName);
      if ((name_length == 1 && find_data.cFileName[0] == '.') ||
          (name_length == 2 && find_data.cFileName[0] == '.' && find_data.cFileName[1] == '.')) {
        // skip . and ..
      } else {
        // std::wcout << (L"\\\\?\\" + tree[index].path + L"\\" + find_data.cFileName).c_str() << std::endl;
        // get attributes to determine if file or directory
        HANDLE hFile{CreateFileW(                                                  //
            (L"\\\\?\\" + tree[index].path + L"\\" + find_data.cFileName).c_str(), //
            FILE_READ_ATTRIBUTES,                                                  // The right to read file attributes.
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,                //
            NULL,                                                                  //
            OPEN_EXISTING,                                                         //
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,             //
            NULL                                                                   //
            )};
        if (hFile == INVALID_HANDLE_VALUE) {
          writeErr(L"1 CreateFile. Could not open target directory for reading.");
          writeErr((L"-- \\\\?\\" + tree[index].path + L"\\" + find_data.cFileName).c_str());
        } else {
          BY_HANDLE_FILE_INFORMATION file_info{};
          BOOL bSuccess{GetFileInformationByHandle(hFile, &file_info)};
          if (bSuccess) {
            tree.push_back({(file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0,     //
                            (file_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0, //
                            0, tree[index].path + L"\\" + find_data.cFileName});

            // tree.push_back({ file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true : false, 0,
            //                 tree[index].path + L"\\" + find_data.cFileName });
            //  std::wstringstream _{};
            //  if (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //    tree.push_back({true, 0, tree[index].path + L"\\" + find_data.cFileName});
            //    _ << L"D ";
            //  } else {
            //    tree.push_back({false, 0, tree[index].path + L"\\" + find_data.cFileName});
            //    _ << L"F ";
            //  }
            //  _ << tree[index].path + L"\\" + find_data.cFileName;
            //  writeOut(_);
          }
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
        goto _FindClose;
      }
      goto _FindNext;
    }

_FindClose:
    FindClose(hFind);
  }

  {
    std::wstringstream _{};
    auto kind{(tree[0].is_directory ? (tree[0].is_symlink ? L"J " : L"D ") : (tree[0].is_symlink ? L"S " : L"F "))};
    _ << kind + tree[0].path;
    for (auto i = 1; i < tree.size(); ++i) {
      auto kind{(tree[i].is_directory ? (tree[i].is_symlink ? L"J " : L"D ") : (tree[i].is_symlink ? L"S " : L"F "))};
      _ << "\n" << kind + tree[i].path;
    }
    writeOut(_);
  }
  return 0;
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  SetConsoleOutputCP(65001);          // set console output code page to utf-8

  switch (argc) { // 0th arg (argc=1) is program path
    case 2:
      return listTree(argv[1]);
    case 3:
      return listTree(argv[1]);
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
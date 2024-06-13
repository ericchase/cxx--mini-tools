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
int watchDirectory(std::filesystem::path const &path) {
  DWORD const buf_size{1024 * 1024 * 64};
  uint8_t *change_buf = new uint8_t[buf_size]{};
  HANDLE hDirectory{INVALID_HANDLE_VALUE};
  BOOL bSuccess{0};
  int delay_ms{1000};

  // If the function succeeds, the return value is a handle to the event object. If the named event object existed
  // before the function call, the function returns a handle to the existing object and GetLastError returns
  // ERROR_ALREADY_EXISTS. If the function fails, the return value is NULL. To get extended error information, call
  // GetLastError.
  OVERLAPPED overlapped{};
  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (overlapped.hEvent == NULL) {
    std::wstringstream _{};
    _ << "0 CreateEvent GetLastError: " << GetLastError();
    writeErr(_);
    goto _Exit;
  }

_Start: {
  bool bStartInit{true};

  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
  hDirectory = CreateFileW( //
      path.c_str(),         //
      FILE_LIST_DIRECTORY,  //  For a directory, the right to list the contents of the directory.
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, //
      NULL,                                                   //
      OPEN_EXISTING,                                          //
      FILE_FLAG_BACKUP_SEMANTICS  // You must set this flag to obtain a handle to a directory.
          | FILE_FLAG_OVERLAPPED, // The file or device is being opened or created for asynchronous I/O.
      NULL                        //
  );
  if (hDirectory == INVALID_HANDLE_VALUE) {
    writeErr(L"1 CreateFile. Could not open target directory for watching.");
    goto _Reset;
  }

  while (true) {
    // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readdirectorychangesw
    bSuccess = ReadDirectoryChangesW( //
        hDirectory,                   //
        change_buf, // A pointer to the DWORD-aligned formatted buffer in which the read results are to be returned.
        buf_size,   // The size of the buffer that is pointed to by the lpBuffer parameter, in bytes.
        TRUE,       // If this parameter is TRUE, the function monitors the directory tree.
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, //
        NULL,        // For asynchronous calls, this parameter is undefined.
        &overlapped, // A pointer to an OVERLAPPED structure that supplies data to be used during asynchronous
                     // operation.
        NULL // A pointer to a completion routine to be called when the operation has been completed or canceled...
    );
    if (bSuccess == 0) {
      writeErr(L"2 ReadDirectoryChanges. Possibly too many changes to track.");
      goto _Reset;
    }

    if (bStartInit) {
      bStartInit = false;
      writeOut(std::wstring(L"S ") + path.wstring());
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
    DWORD signal{WaitForSingleObject(overlapped.hEvent, INFINITE)};
    switch (signal) {
      case WAIT_OBJECT_0: {
        DWORD bytes_transferred{};
        // https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getoverlappedresult
        bSuccess = GetOverlappedResult(hDirectory, &overlapped, &bytes_transferred, TRUE);
        if (bSuccess == 0) {
          std::wstringstream _{};
          _ << "0 GetOverlappedResult GetLastError: " << GetLastError();
          writeErr(_);
          goto _Reset;
        }

        std::wstringstream out_changes{};
        // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-file_notify_information
        FILE_NOTIFY_INFORMATION *event{(FILE_NOTIFY_INFORMATION *)change_buf};
        for (;;) {
          switch (event->Action) {
            case FILE_ACTION_ADDED:
              out_changes << "C ";
              break;
            case FILE_ACTION_REMOVED:
              out_changes << "D ";
              break;
            case FILE_ACTION_MODIFIED:
              out_changes << "M ";
              break;
            case FILE_ACTION_RENAMED_OLD_NAME:
              out_changes << "R ";
              break;
            case FILE_ACTION_RENAMED_NEW_NAME:
              out_changes << "\x09";
              break;
          }

          {
            DWORD name_len{event->FileNameLength / sizeof(wchar_t)};
            out_changes << std::wstring{event->FileName, name_len};
          }

          // Are there more events to handle?
          if (event->NextEntryOffset) {
            *((uint8_t **)&event) += event->NextEntryOffset;
          } else {
            break;
          }
        }
        if (!out_changes.str().empty()) {
          writeOut(out_changes);
        }
      } break;
      case WAIT_FAILED: {
        break;
      }
    }
    // If the function succeeds, the return value is nonzero.
    // If the function fails, the return value is zero. To get extended error information, call GetLastError.
    bSuccess = ResetEvent(overlapped.hEvent);
    if (bSuccess == 0) {
      std::wstringstream _{};
      _ << "0 ResetEvent GetLastError: " << GetLastError();
      writeErr(_);
      goto _Exit;
    }
  }
  goto _Exit;
}

_Reset: {
  CloseHandle(hDirectory);
  Sleep(delay_ms);
  goto _Start;
}

_Exit:
  CloseHandle(hDirectory);
  return 0;
}

int _tmain(int argc, TCHAR *argv[]) { // requires <tchar.h>
  SetConsoleOutputCP(65001);          // set console output code page to utf-8

  if (argc > 1) {
    return watchDirectory(resolvePath(std::filesystem::path{argv[1]}));
  } else {
    std::wstringstream wss{};
    wss << Help << "\n\n" << Break << "\n\n" << Info;
    writeOut(wss);
  }
  return 0;
}
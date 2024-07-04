auto Help{LR"(List files and directories in target directory (non-recursive).

lsd <Path> [<Filter>]

  Path - Relative or absolute directory path for target directory.
  Filter - Path name with wildcards used to filter results.

Standard Output
  <Code> <Name>

    Code - A code to distinguish between files and directories.
    Name - File or directory name within target directory.

Codes
  F - File
  D - Directory

Standard Error
  <Error Code> <Path>

Error Codes
  1 - CreateFileW. Could not open target directory for watching.
  2 - FindFirstFileW. No files found.

Error Code 0
  <Error Code> <Function Name> GetLastError: <GetLastError>
  0 - Check Windows System Error Codes for the GetLastError value.)"};
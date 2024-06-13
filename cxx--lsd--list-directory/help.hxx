auto Help{LR"(List files and directories in target directory (non-recursive).

lsd <Path> [<Filter>]

  Path - Relative or absolute directory path for target directory.
  Filter - Path name with wildcards used to filter results.

Standard Output
  <Code> <Name>

    Code - A code to distinguish between files and directories.
    Name - File or directory name found within target directory (non-recursive).

Codes
  F - File
  D - Directory

Standard Error
  <Error Code> <Short Description>

Error Codes
  0 - Check Windows System Error Codes for the GetLastError value.
  1 - CreateFile. Could not open found file or directory.
  2 - FindFirstFile. No files found.)"};
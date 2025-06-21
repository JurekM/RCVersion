# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RCVersion is a Windows console application that modifies version numbers in Windows Resource (.RC) files. It's designed for automated build systems to update version information in C++ projects.

## Build Commands

This is a Visual Studio C++ project using VS2015 toolset (v140).

**Build the main application:**
```bash
# Open in Visual Studio and build, or use MSBuild
msbuild RCVersion.sln /p:Configuration=Debug /p:Platform=x64
msbuild RCVersion.sln /p:Configuration=Release /p:Platform=x64
```

**Build and run tests:**
```bash
# Build test project
msbuild RCVersionTests/RCVersionTests.vcxproj /p:Configuration=Debug /p:Platform=x64
# Run tests (uses Google Test framework)
./_Debug-x64/RCVersionTests.exe
```

**Available configurations:**
- Debug/Release
- Win32/x64 platforms
- Executables output to: `_Debug-Win32/`, `_Debug-x64/`, `_Release-Win32/`, `_Release-x64/`

## Architecture

**Core Components:**
- **RCVersionOptions**: Command-line parsing with support for `/m:`, `/n:`, `/b:`, `/r:`, `/o:`, `/v:` parameters
- **RCFileHandler**: RC file I/O operations with Unicode/ANSI detection
- **RCUpdater**: Template-based version parsing and updating (supports both char/wchar_t)
- **ILogger/Logger**: Interface-based logging with verbosity levels (0-9)

**Key Files:**
- `RCVersion/main.cpp`: Application entry point and workflow coordination
- `RCVersion/RCVersionOptions.h/.cpp`: Command-line parameter handling
- `RCVersion/RCFileHandler.h/.cpp`: File loading/saving operations
- `RCVersion/RCUpdater.h`: Core version updating logic (header-only template)
- `RCVersion/ILogger.h`, `RCVersion/Logger.h`: Logging infrastructure

**Utility Classes:**
- `AutoFree.h`: RAII wrapper for malloc/free
- `AutoHClose.h`: RAII wrapper for Windows HANDLE cleanup
- `MessageBuffer.h`: Unicode/ANSI string conversion

## Testing

Uses Google Test framework with comprehensive test coverage (140+ tests). Tests are organized into:

**Core Functionality Tests:**
- **HandlerTests.cpp**: RC file processing tests with Unicode/ANSI/BOM handling
- **HelperTests.cpp**: Utility class tests for RAII wrappers
- **OptionsTests.cpp**: Command-line parsing tests
- **UpdaterTests.cpp**: Version updating logic tests

**Edge Case and Error Handling Tests:**
- **FileHandlerErrorTests.cpp**: File I/O error conditions and edge cases
- **OptionsEdgeCaseTests.cpp**: Command-line parsing edge cases and environment variables
- **LoggerTests.cpp**: Logging verbosity levels and error handling
- **MessageBufferEdgeCaseTests.cpp**: Unicode handling and format string edge cases
- **RCUpdaterEdgeCaseTests.cpp**: Version parsing and buffer replacement edge cases
- **UnicodeFileTests.cpp**: Comprehensive Unicode file encoding tests
- **IntegrationTests.cpp**: End-to-end workflow testing

**Test Infrastructure:**
- All test fixtures use helper methods for temporary file/directory management
- Automatic cleanup of test resources via RAII patterns
- Comprehensive error condition testing without crashes or undefined behavior
- Support for various RC file formats, encodings, and edge cases

## Code Patterns

- **Template-based text processing**: RCUpdater supports both narrow and wide character strings
- **RAII resource management**: Automatic cleanup for memory and file handles
- **Interface-based logging**: ILogger interface allows different logging backends
- **Error handling**: Consistent error codes and logging throughout
- **Single responsibility**: Each class has focused functionality

## Important Notes

- The application processes Windows RC files specifically for VERSIONINFO blocks
- Supports both comma and period-separated version strings in input
- Always outputs comma-space separated format (e.g., "1, 2, 3, 0")
- Handles standard Visual Studio generated RC files but has limitations with embedded comments
- Designed for build automation with environment variable expansion support
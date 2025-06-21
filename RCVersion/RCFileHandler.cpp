#include "stdafx.h"
#include "RCFileHandler.h"
#include "RCUpdater.h"
#include "wil/resource.h"

RCFileHandler::RCFileHandler(ILogger &rlogger)
  : ilogger(rlogger)
  , logger(rlogger)
  , error(0)
{
}

RCFileHandler::~RCFileHandler()
{
}

bool RCFileHandler::UpdateFile(const wchar_t *inpath, const wchar_t *outpath, int major, int minor, int build, int revision)
{
  logger.Log(logDetail, L"UpdateFile(%s,%s)", NN(inpath), NN(outpath));

  std::vector<unsigned char> buffer;

  if (!LoadFile(inpath, 1024, buffer))
  {
    return false;
  }

  int flags = IS_TEXT_UNICODE_UNICODE_MASK;
  bool isUnicode = 0 != IsTextUnicode(buffer.data(), int(min(buffer.size(), size_t{256})), &flags);

  unsigned changes;
  if (isUnicode)
  {
    changes = UpdateBuffer(reinterpret_cast<wchar_t*>(buffer.data()), buffer.size() / sizeof(wchar_t), major, minor, build, revision);
  }
  else
  {
    changes = UpdateBuffer(reinterpret_cast<char*>(buffer.data()), buffer.size() / sizeof(char), major, minor, build, revision);
  }

  if (0 == changes)
  {
    logger.Log(logNormal, L"No changes made to [%s], file [%s] not modified.", NN(inpath), NN(outpath));
    error = ERROR_FILE_CORRUPT;
    return false;
  }

  logger.Log(logNormal, L"%u changes made to [%s], writing file [%s].", changes, NN(inpath), NN(outpath));
  unsigned outBytes = unsigned(isUnicode ? wcslen(reinterpret_cast<wchar_t*>(buffer.data()))*sizeof(wchar_t) : strlen(reinterpret_cast<char*>(buffer.data()))*sizeof(char));

  if (!SaveFile(outpath, buffer.data(), outBytes))
  {
    return false;
  }

  return true;
}

unsigned RCFileHandler::RCFileHandler::UpdateBuffer(char* buffer, size_t chars, int major, int minor, int build, int revision) const
{
  logger.Log(logDetail, L"UpdateBuffer<char>(...,%u)", unsigned(chars));
  RCUpdater<char> updater{ilogger};
  updater.verbosity = logger.Verbosity();
  unsigned changes = updater.UpdateVersion(buffer, chars, major, minor, build, revision);
  return changes;
}

unsigned RCFileHandler::RCFileHandler::UpdateBuffer(wchar_t* buffer, size_t chars, int major, int minor, int build, int revision) const
{
  logger.Log(logDetail, L"UpdateBuffer<wchar>(...,%u)", unsigned(chars));
  RCUpdater<wchar_t> updater{ilogger};
  updater.verbosity = logger.Verbosity();
  unsigned changes = updater.UpdateVersion(buffer, chars, major, minor, build, revision);
  return changes;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileHandler::LoadFile(const wchar_t* path, size_t padding, std::vector<unsigned char>& buffer)
{
  logger.Log(logDetail, L"Reading file [%s]...", path);
  if (!path || !*path)
  {
    return logger.Error(error = ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Load: Input file path must not be empty");
  }

  wil::unique_hfile hFile(CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, 0, nullptr));
  if (!hFile)
  {
    return logger.Error(error = GetLastError(), L"*** RCFileUpdater::Load: Cannot open input file", path);
  }

  ULARGE_INTEGER li{};
  li.LowPart = GetFileSize(hFile.get(), &li.HighPart);
  if (0 != li.HighPart || (0x7FFFFFFF - padding) <= li.LowPart)
  {
    return logger.Error(error = ERROR_FILE_CORRUPT, L"*** RCFileUpdater::Load: File too large", path);
  }

  if (padding < 2)
  {
    padding = 2;
  }

  size_t totalSize = li.LowPart + padding;
  try
  {
    buffer.resize(totalSize);
  }
  catch (const std::bad_alloc&)
  {
    return logger.Error(error = ERROR_OUTOFMEMORY, L"*** RCFileUpdater::Load: File too large", path);
  }

  DWORD readBytes{};
  BOOL ok = ReadFile(hFile.get(), buffer.data(), DWORD(li.LowPart), &readBytes, nullptr);
  if (!ok || li.LowPart < readBytes)
  {
    buffer.clear();
    return logger.Error(error = GetLastError(), L"*** RCFileUpdater::Load: Cannot read input file", path);
  }

  buffer[readBytes] = 0;
  buffer[readBytes + 1] = 0;
  return true;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
bool RCFileHandler::SaveFile(const wchar_t* path, void* buffer, size_t bytes)
{
  logger.Log(logDetail, L"Writing file [%s]...", path);
  if (!path || !*path)
  {
    return logger.Error(error = ERROR_INVALID_PARAMETER, L"*** RCFileUpdater::Save: Output file path must not be empty.");
  }

  wil::unique_hfile hFile(CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr, CREATE_ALWAYS, 0, nullptr));
  if (!hFile)
  {
    return logger.Error(error = GetLastError(), L"*** RCFileUpdater::Save: Cannot open output file [%s]", path);
  }

  DWORD writeBytes{};
  BOOL ok = WriteFile(hFile.get(), buffer, DWORD(bytes), &writeBytes, nullptr);
  if (!ok || bytes != writeBytes)
  {
    return logger.Error(error = GetLastError(), L"*** RCFileUpdater::Save: Cannot write output file [%s]", path);
  }

  return true;
}

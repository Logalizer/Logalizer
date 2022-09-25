#include "application.h"
#ifdef _WIN32
#include <windows.h>  //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>  //readlink
#endif

std::filesystem::path getExePath()
{
#ifdef _WIN32
   wchar_t path[MAX_PATH] = {0};
   GetModuleFileNameW(NULL, path, MAX_PATH);
   return path;
#else
   char result[PATH_MAX];
   ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
   return std::string(result, (count > 0) ? count : 0);
#endif
}

std::filesystem::path getExeDir()
{
   return getExePath().parent_path();
}

std::filesystem::path getLogalizerPath()
{
#ifdef _WIN32
   return getExeDir() / "Logalizer.exe";
#else
   return getExeDir() / "Logalizer";
#endif
}

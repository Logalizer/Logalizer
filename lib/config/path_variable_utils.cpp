#include <filesystem>

#ifdef _WIN32
#include <windows.h>  //GetModuleFileNameW
#else
#include <unistd.h>  //readlink
#include <climits>
#endif

namespace fs = std::filesystem;
namespace Logalizer::Config::Utils {

std::string get_exe_path()
{
#ifdef _WIN32
   wchar_t path[MAX_PATH] = {0};
   GetModuleFileNameW(NULL, path, MAX_PATH);
   return fs::path(path).string();
#else
   char result[PATH_MAX];
   ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
   return std::string(result, (count > 0) ? count : 0);
#endif
}

std::string get_exe_dir()
{
   fs::path exepath(get_exe_path());
   return exepath.parent_path().string();
}

}  // namespace Logalizer::Config::Utils

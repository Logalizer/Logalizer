#include "path_variable_utils.h"
#include <sys/stat.h>

namespace Logalizer::Config::Utils {

static const std::string VAR_FILE_DIR_NAME = "${fileDirname}";
static const std::string VAR_FILE_BASE_NO_EXTENSION = "${fileBasenameNoExtension}";
static const std::string VAR_FILE_BASE_WITH_EXTENSION = "${fileBasename}";

std::pair<std::string, std::string> dir_file(std::string const &basefile)
{
   size_t end = basefile.find_last_of("/\\");
   std::string dirvalue = ".";
   if (end != std::string::npos) {
      dirvalue = basefile.substr(0, end);
      if (dirvalue.length() == 0) dirvalue = basefile;
   }

   size_t start = basefile.find_last_of("/\\");
   std::string filevalue = basefile;
   if (start != std::string::npos) {
      filevalue = basefile.substr(start + 1, std::string::npos);
   }

   std::pair<std::string, std::string> dirfile{dirvalue, filevalue};
   return dirfile;
}

std::string without_extension(std::string const &file)
{
   auto [dir, filename] = dir_file(file);
   size_t end = filename.find_last_of(".");
   std::string wo_ext = filename;
   if (end != std::string::npos) wo_ext = filename.substr(0, end);
   return wo_ext;
}

void replace_all(std::string *input, std::string const &token, std::string const &replace)
{
   size_t pos = input->find(token);
   while (pos != std::string::npos) {
      input->replace(pos, token.length(), replace);
      pos = input->find(token);
   }
}

void replace_paths(std::string *input, std::string const &dir, std::string const &file)
{
   replace_all(input, VAR_FILE_DIR_NAME, dir);
   replace_all(input, VAR_FILE_BASE_WITH_EXTENSION, file);
   replace_all(input, VAR_FILE_BASE_NO_EXTENSION, without_extension(file));
}

bool is_directory(std::string const &path)
{
   struct stat info;
   return (stat(path.c_str(), &info) == 0) && (info.st_mode & S_IFDIR);
}

bool mkdir(std::string const &path)
{
   if (is_directory(path)) {
      return true;
   }
   bool result = false;
   std::string stripped_path = path;
#ifdef _WIN32
   replace_all(&stripped_path, "/", "\\");
   result = system((std::string("mkdir \"") + path + "\"").c_str()) == 0;
#else
   replace_all(&stripped_path, "\\", "/");
   result = system((std::string("mkdir -p \"") + path + "\"").c_str()) == 0;
#endif
   return result;
}

}  // namespace Logalizer::Config::Utils

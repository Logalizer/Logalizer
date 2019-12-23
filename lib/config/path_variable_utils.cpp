#include "path_variable_utils.h"

namespace Logalizer::Config::Utils {

static const std::string VAR_FILE_DIR_NAME = "${fileDirname}";
static const std::string VAR_FILE_BASE_NO_EXTENSION = "${fileBasenameNoExtension}";
static const std::string VAR_FILE_BASE_WITH_EXTENSION = "${fileBasename}";

std::pair<std::string, std::string> getDirFile(std::string const &basefile)
{
   size_t end = basefile.find_last_of("/\\");
   std::string dirvalue = ".";
   if (end != std::string::npos) dirvalue = basefile.substr(0, end);

   size_t start = basefile.find_last_of("/\\");
   std::string filevalue = basefile;
   if (start != std::string::npos) filevalue = basefile.substr(start, std::string::npos);

   std::pair<std::string, std::string> dirfile{dirvalue, filevalue};
   return dirfile;
}

std::string fileNameWithoutExtension(std::string const &file)
{
   size_t end = file.find_last_of(".");
   std::string fileName = file;
   if (end != std::string::npos) fileName = file.substr(0, end);
   return fileName;
}

void findAndReplace(std::string *input, std::string const &token, std::string const &replace)
{
   size_t pos = input->find(token);
   while (pos != std::string::npos) {
      input->replace(pos, token.length(), replace);
      pos = input->find(token);
   }
}

void replaceStringVariables(std::string *input, std::string const &dir, std::string const &file)
{
   findAndReplace(input, VAR_FILE_DIR_NAME, dir);
   findAndReplace(input, VAR_FILE_BASE_WITH_EXTENSION, file);
   findAndReplace(input, VAR_FILE_BASE_NO_EXTENSION, fileNameWithoutExtension(file));
}

void mkdir(std::string path)
{
   #ifdef _WIN32
      findAndReplace(&path, "/", "");
      system((std::string("mkdir ") + path).c_str());
   #else
      findAndReplace(&path, "\\", "");
      system((std::string("mkdir -p ") + path).c_str());
   #endif
}

}  // namespace Logalizer::Config::Utils

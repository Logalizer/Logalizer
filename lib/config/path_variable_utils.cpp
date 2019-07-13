#include "path_variable_utils.h"

namespace Logalizer::Config::Utils {

static const std::string VAR_FILE_DIR_NAME = "${fileDirname}";
static const std::string VAR_FILE_BASE_NO_EXTENSION = "${fileBasenameNoExtension}";

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

void findAndReplace(std::string *input, std::string const &token, std::string const &replace)
{
   size_t pos = input->find(token);
   if (pos != std::string::npos) {
      input->replace(pos, token.length(), replace);
   }
}

void replaceStringVariables(std::string *input, std::string const &dir, std::string const &file)
{
   findAndReplace(input, VAR_FILE_DIR_NAME, dir);
   findAndReplace(input, VAR_FILE_BASE_NO_EXTENSION, file);
}

}  // namespace Logalizer::Config::Utils

#include "path_variable_utils.h"
#include <sys/stat.h>
#include <filesystem>

namespace fs = std::filesystem;
namespace Logalizer::Config::Utils {

static const std::string VAR_FILE_DIR_NAME = "${fileDirname}";
static const std::string VAR_FILE_BASE_NO_EXTENSION = "${fileBasenameNoExtension}";
static const std::string VAR_FILE_BASE_WITH_EXTENSION = "${fileBasename}";

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
   replace_all(input, VAR_FILE_BASE_NO_EXTENSION, fs::path(file).replace_extension(""));
}

}  // namespace Logalizer::Config::Utils

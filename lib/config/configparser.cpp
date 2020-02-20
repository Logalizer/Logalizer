#include "configparser.h"
#include <iostream>
#include <string>
#include "path_variable_utils.h"

namespace Logalizer::Config {

ConfigParser::ConfigParser(std::string config_file) : config_file_{std::move(config_file)}
{
}

void ConfigParser::updateRelativePaths(std::string const &log_file)
{
   using namespace Utils;
   auto [dir, file] = getDirFile(log_file);
   std::for_each(begin(execute_commands_), end(execute_commands_),
                 [dir = dir, file = file](auto &command) { replaceStringVariables(&command, dir, file); });

   replaceStringVariables(&backup_file_, dir, file);
   replaceStringVariables(&translation_file_, dir, file);
}
}  // namespace Logalizer::Config

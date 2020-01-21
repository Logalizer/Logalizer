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
   replaceStringVariables(&generate_uml_command_, dir, file);

   replaceStringVariables(&backup_file_, dir, file);
   replaceStringVariables(&uml_file_, dir, file);
}
}  // namespace Logalizer::Config

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
   std::pair<std::string, std::string> dirfile = getDirFile(log_file);
   replaceStringVariables(&generate_uml_command_, dirfile.first, dirfile.second);

   replaceStringVariables(&backup_file_, dirfile.first, dirfile.second);
   replaceStringVariables(&uml_file_, dirfile.first, dirfile.second);
}
}  // namespace Logalizer::Config

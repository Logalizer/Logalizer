#include "configparser.h"
#include <iostream>
#include <string>
#include "path_variable_utils.h"

namespace Logalizer::Config {

ConfigParser::ConfigParser(std::string config_file) : config_file_{std::move(config_file)}
{
}

void ConfigParser::update_relative_paths(std::string const &log_file)
{
   using namespace Utils;
   const auto [dir, file] = dir_file(log_file);
   std::for_each(begin(execute_commands_), end(execute_commands_),
                 [dir = dir, file = file](auto &command) { replace_paths(&command, dir, file); });

   replace_paths(&backup_file_, dir, file);
   replace_paths(&translation_file_, dir, file);
}
}  // namespace Logalizer::Config

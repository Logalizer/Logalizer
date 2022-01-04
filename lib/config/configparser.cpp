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

void ConfigParser::load_configurations()
{
   try {
      load_translations();
      load_translation_file();
   }
   catch (std::exception &e) {
      std::cout << e.what() << '\n';
      throw;
   }
   try {
      load_wrap_text();
   }
   catch (...) {
   }
   try {
      load_blacklists();
   }
   catch (...) {
   }
   try {
      load_delete_lines();
   }
   catch (...) {
   }
   try {
      load_replace_words();
   }
   catch (...) {
   }
   try {
      load_execute();
   }
   catch (...) {
   }
   try {
      load_backup_file();
   }
   catch (...) {
   }
   try {
      load_auto_new_line();
   }
   catch (...) {
      auto_new_line_ = true;
   }
}
}  // namespace Logalizer::Config

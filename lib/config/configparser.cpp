#include "configparser.h"
#include <iostream>
#include <regex>
#include <string>

namespace Logalizer::Config {

ConfigParser::ConfigParser(std::string config_file) : config_file_{std::move(config_file)}
{
}

void ConfigParser::update_path_variables()
{
   auto update_path_vars = [&, this](std::string *input) {
      *input = std::regex_replace(*input, std::regex(VAR_FILE_DIR_NAME), input_file_details_.dir);
      *input = std::regex_replace(*input, std::regex(VAR_FILE_BASE_WITH_EXTENSION), input_file_details_.file);
      *input = std::regex_replace(*input, std::regex(VAR_FILE_BASE_NO_EXTENSION), input_file_details_.file_no_ext);
   };
   std::for_each(begin(execute_commands_), end(execute_commands_), [&](auto &command) { update_path_vars(&command); });

   update_path_vars(&backup_file_);
   update_path_vars(&translation_file_);
}

void ConfigParser::load_configurations()
{
   try {
      // disabled categories are used in translations
      load_disabled_categories();
   }
   catch (...) {
   }
   try {
      load_translation_file();
      load_translations();
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

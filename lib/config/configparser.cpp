#include "configparser.h"
#include <iostream>
#include <regex>
#include <string>
#include "path_variable_utils.h"

namespace Logalizer::Config {

void ConfigParser::update_path_variables()
{
   auto update_path_vars = [&, this](std::string *input) {
      *input = std::regex_replace(*input, std::regex(VAR_FILE_DIR_NAME), input_file_details_.dir);
      *input = std::regex_replace(*input, std::regex(VAR_FILE_BASE_WITH_EXTENSION), input_file_details_.file);
      *input = std::regex_replace(*input, std::regex(VAR_FILE_BASE_NO_EXTENSION), input_file_details_.file_no_ext);
      *input = std::regex_replace(*input, std::regex(VAR_EXE_DIR_NAME), Utils::get_exe_dir());
   };

   std::for_each(begin(execute_commands_), end(execute_commands_), [&](auto &command) { update_path_vars(&command); });
   update_path_vars(&backup_file_);
   update_path_vars(&translation_file_);
}

bool ConfigParser::is_disabled(const std::string &category)
{
   return std::any_of(cbegin(disabled_categories_), cend(disabled_categories_), [&category](auto const &dCategory) {
      if (category == dCategory) return true;
      return false;
   });
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
   catch (...) {
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
   try {
      update_path_variables();
   }
   catch (std::exception &e) {
      std::cerr << "[warn] Special variables for path not updated " << e.what() << "\n";
   }
}
}  // namespace Logalizer::Config

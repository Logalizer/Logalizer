#include "jsonconfigparser.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace Logalizer::Config {

using json = nlohmann::json;

namespace details {

template <class T>
T get_value(json const &config, std::string const &name)
{
   T value{};
   auto found = config.find(name);

   if (found == config.end()) {
      std::cerr << "Element " << name << " not found\n";
   }
   else {
      value = found.value();
   }

   return value;
}

template <class T>
T get_value_or(json const &config, std::string const &name, T value)
{
   const auto found = config.find(name);

   if (found == config.end()) {
      std::cerr << "Element " << name << " not found\n";
   }
   else {
      value = found.value();
   }

   return value;
}

std::vector<variable> get_variables(json const &jvariables)
{
   std::vector<variable> variables;
   for (const auto &[key, value] : jvariables.items()) {
      variables.emplace_back(variable{value["startswith"], value["endswith"]});
   }
   return variables;
}

std::vector<std::string> load_array(json const &config, std::string const &name)
{
   std::vector<std::string> array;
   json j_bl = get_value<json>(config, name);

   for (const auto &[key, value] : j_bl.items()) {
      array.push_back(value);
   }
   return array;
}

std::vector<translation> load_translations(json const &config, std::string const &name,
                                           std::vector<std::string> const &disabled_categories)
{
   std::vector<translation> translations;
   translation tr;
   const json j_tr = get_value<json>(config, name);

   for (const auto &[key, value] : j_tr.items()) {
      // key : 0, 1, 2...
      // value {tranlation_element1, tranlation_element2, ...}

      const json entry = value;
      const std::string category = entry["category"];

      const bool is_disabled =
          std::any_of(cbegin(disabled_categories), cend(disabled_categories), [&category](auto const &dCategory) {
             if (category == dCategory) return true;
             return false;
          });
      if (is_disabled) continue;

      // translations.emplace_back(entry["category"], loadArray(entry, "patterns"), entry["print"],
      //                                getVariables(entry["variables"]));

      tr.category = entry["category"];
      tr.patterns = load_array(entry, "patterns");
      tr.print = entry["print"];
      tr.repeat = (entry["repeat"] == "false") ? false : true;
      tr.variables = get_variables(entry["variables"]);
      if (entry["count"] == "scoped") {
         tr.count = count_type::scoped;
      }
      else if (entry["count"] == "global") {
         tr.count = count_type::global;
      }
      else {
         tr.count = count_type::none;
      }
      translations.push_back(std::move(tr));
   }
   return translations;
}
}  // namespace details

JsonConfigParser::JsonConfigParser(std::string const &config_file) : ConfigParser(config_file)
{
   if (config_file_.empty()) config_file_ = "config.json";
}

JsonConfigParser::JsonConfigParser(nlohmann::json config) : ConfigParser("")
{
   config_ = std::move(config);
}

void JsonConfigParser::load_config_file()
{
   std::ifstream file(config_file_);
   file >> config_;
   std::cout << "configuration loaded from " << config_file_ << '\n';
}

void JsonConfigParser::load_all_configurations()
{
   load_translations();
   load_wrap_text();
   load_blacklists();
   load_delete_lines();
   load_replace_words();
   load_execute();
   load_translation_file();
   load_backup_file();
   load_auto_new_line();
}

void JsonConfigParser::load_translations()
{
   disabled_categories_ = details::load_array(config_, TAG_DISABLE_CATEGORY);
   translations_ = details::load_translations(config_, TAG_TRANSLATIONS, disabled_categories_);
}

void JsonConfigParser::load_wrap_text()
{
   try {
      wrap_text_pre_ = std::vector<std::string>(config_[TAG_WRAPTEXT_PRE]);
   }
   catch (...) {
   }
   try {
      wrap_text_post_ = std::vector<std::string>(config_[TAG_WRAPTEXT_POST]);
   }
   catch (...) {
   }
}

void JsonConfigParser::load_blacklists()
{
   try {
      blacklists_ = std::vector<std::string>(config_[TAG_BLACKLIST]);
   }
   catch (...) {
   }
}

void JsonConfigParser::load_delete_lines()
{
   const std::vector<std::string> deletors = details::load_array(config_, TAG_DELETE_LINES);

   for (auto const &entry : deletors) {
      if (entry.find_first_of("[\\^$.|?*+") != std::string::npos) {
         delete_lines_regex_.emplace_back(
             entry, std::regex_constants::grep | std::regex_constants::nosubs | std::regex_constants::optimize);
      }
      else {
         delete_lines_.emplace_back(entry);
      }
   }

   if (delete_lines_regex_.size()) {
      std::cerr << "[Warning] Use of regex in " << TAG_DELETE_LINES << " is a lot slower. Use normal search instead,\n";
      for (auto const &entry : deletors) {
         if (entry.find_first_of("[\\^$.|?*+") != std::string::npos) {
            std::cout << "  " << entry << '\n';
         }
      }
   }
}

void JsonConfigParser::load_replace_words()
{
   const json j_tr = details::get_value<json>(config_, TAG_REPLACE_WORDS);
   for (const auto &[key, value] : j_tr.items()) {
      replace_words_.emplace_back(key, value);
   }
}

void JsonConfigParser::load_execute()
{
   execute_commands_ = details::load_array(config_, TAG_EXECUTE);
}

void JsonConfigParser::load_translation_file()
{
   translation_file_ = details::get_value<std::string>(config_, TAG_TRANSLATION_FILE);
}

void JsonConfigParser::load_backup_file()
{
   backup_file_ = details::get_value<std::string>(config_, TAG_BACKUP_FILE);
}

void JsonConfigParser::load_auto_new_line()
{
   auto_new_line_ = details::get_value_or<bool>(config_, TAG_AUTO_NEW_LINE, true);
}

}  // namespace Logalizer::Config

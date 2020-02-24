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
   T value;
   auto found = config.find(name);

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
   for (auto &[key, value] : jvariables.items()) {
      variables.emplace_back(variable{value["startswith"], value["endswith"]});
   }
   return variables;
}

std::vector<std::string> load_array(json const &config, std::string const &name)
{
   std::vector<std::string> array;
   json j_bl = get_value<json>(config, name);

   for (auto &[key, value] : j_bl.items()) {
      array.push_back(value);
   }
   return array;
}

std::vector<translation> load_translations(json const &config, std::string const &name,
                                           std::vector<std::string> disabled_categories)
{
   std::vector<translation> translations;
   translation tr;
   json j_tr = get_value<json>(config, name);

   for (auto &[key, value] : j_tr.items()) {
      // key : 0, 1, 2...
      // value {tranlation_element1, tranlation_element2, ...}

      json entry = value;
      std::string category = entry["category"];

      bool is_disabled =
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
      translations.push_back(std::move(tr));
   }
   return translations;
}
}  // namespace details

JsonConfigParser::JsonConfigParser(std::string const &config_file) : ConfigParser(config_file)
{
   if (config_file_.empty()) config_file_ = "config.json";
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
}

JsonConfigParser::~JsonConfigParser()
{
}

void JsonConfigParser::load_translations()
{
   disabled_categories_ = details::load_array(config_, TAG_DISABLE_CATEGORY);
   translations_ = details::load_translations(config_, TAG_TRANSLATIONS, disabled_categories_);
}

void JsonConfigParser::load_wrap_text()
{
   wrap_text_pre_ = details::load_array(config_, TAG_WRAPTEXT_PRE);
   wrap_text_post_ = details::load_array(config_, TAG_WRAPTEXT_POST);
}

void JsonConfigParser::load_blacklists()
{
   blacklists_ = details::load_array(config_, TAG_BLACKLIST);
}

void JsonConfigParser::load_delete_lines()
{
   std::vector<std::string> deletors = details::load_array(config_, TAG_DELETE_LINES);

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
   json j_tr = details::get_value<json>(config_, TAG_REPLACE_WORDS);
   for (auto &[key, value] : j_tr.items()) {
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

}  // namespace Logalizer::Config

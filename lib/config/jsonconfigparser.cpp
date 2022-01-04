#include "jsonconfigparser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "configparser.h"

namespace Logalizer::Config {

using json = nlohmann::json;

/*
 * config.at() throws an exception if tag not found
 * ConfigParser will decide which exceptions to ignore
 */

namespace details {

template <class T>
T get_value_or(json const &config, std::string const &name, T value)
{
   const auto found = config.find(name);
   if (found != config.end()) {
      value = found.value();
   }
   return value;
}

std::vector<variable> get_variables(json const &config)
{
   std::vector<variable> variables;
   const auto &jvariables = get_value_or(config, "variables", json{});
   for (const auto &item : jvariables.items()) {
      variables.emplace_back(variable{item.value().at("startswith"), item.value().at("endswith")});
   }
   return variables;
}

std::vector<std::string> load_array(json const &config, std::string const &name)
{
   std::vector<std::string> array;
   json jarray = config.at(name);

   for (const auto &item : jarray.items()) {
      array.push_back(item.value());
   }
   return array;
}

std::vector<translation> load_translations(json const &config, std::string const &name,
                                           std::vector<std::string> const &disabled_categories)
{
   std::vector<translation> translations;

   for (const auto &item : config.at(name).items()) {
      // item {tranlation_element1, tranlation_element2, ...}

      const json &jtranslation = item.value();
      const std::string category = get_value_or(jtranslation, TAG_CATEGORY, std::string{});

      const bool is_disabled =
          std::any_of(cbegin(disabled_categories), cend(disabled_categories), [&category](auto const &dCategory) {
             if (category == dCategory) return true;
             return false;
          });
      if (is_disabled) continue;

      translation tr;
      tr.category = category;
      try {
         tr.patterns = load_array(jtranslation, TAG_PATTERNS);
      }
      catch (...) {
         std::cerr << "[warn]: patterns not defined\n";
         continue;
      }
      if (tr.patterns.empty()) {
         std::cerr << "[warn]: patterns empty\n";
         continue;
      }
      tr.print = get_value_or(jtranslation, TAG_PRINT, std::string{});
      if (tr.print.empty()) {
         std::cerr << "[warn]: print not defined or empty\n";
         continue;
      }
      tr.repeat = get_value_or(jtranslation, TAG_REPEAT, true);
      tr.variables = get_variables(jtranslation);
      std::string ct = get_value_or(jtranslation, TAG_COUNT, std::string{});
      if (ct == TAG_COUNT_SCOPED) {
         tr.count = count_type::scoped;
      }
      else if (ct == TAG_COUNT_GLOBAL) {
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

void JsonConfigParser::read_config_file()
{
   std::ifstream file(config_file_);
   file >> config_;
   std::cout << "configuration loaded from " << config_file_ << '\n';
}

void JsonConfigParser::load_disabled_categories()
{
   disabled_categories_ = details::load_array(config_, TAG_DISABLE_CATEGORY);
}

void JsonConfigParser::load_translations()
{
   translations_ = details::load_translations(config_, TAG_TRANSLATIONS, disabled_categories_);
}

void JsonConfigParser::load_wrap_text()
{
   try {
      wrap_text_pre_ = std::vector<std::string>(config_.at(TAG_WRAPTEXT_PRE));
   }
   catch (...) {
   }
   try {
      wrap_text_post_ = std::vector<std::string>(config_.at(TAG_WRAPTEXT_POST));
   }
   catch (...) {
   }
}

void JsonConfigParser::load_blacklists()
{
   try {
      blacklists_ = std::vector<std::string>(config_.at(TAG_BLACKLIST));
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
      std::cerr << "[warn] Use of regex in " << TAG_DELETE_LINES << " is a lot slower. Use normal search instead,\n";
      for (auto const &entry : deletors) {
         if (entry.find_first_of("[\\^$.|?*+") != std::string::npos) {
            std::cout << "  " << entry << '\n';
         }
      }
   }
}

void JsonConfigParser::load_replace_words()
{
   const json j_tr = config_.at(TAG_REPLACE_WORDS);
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
   translation_file_ = config_.at(TAG_TRANSLATION_FILE);
}

void JsonConfigParser::load_backup_file()
{
   backup_file_ = config_.at(TAG_BACKUP_FILE);
}

void JsonConfigParser::load_auto_new_line()
{
   auto_new_line_ = config_.at(TAG_AUTO_NEW_LINE);
}

}  // namespace Logalizer::Config

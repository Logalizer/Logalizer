#include "jsonconfigparser.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include "config_types.h"
#include "configparser.h"
#include "csv.h"

namespace Logalizer::Config {

using json = nlohmann::json;

/*
 * config.at() throws an exception if tag not found
 * ConfigParser will decide which exceptions to ignore
 */

template <class T>
T Logalizer::Config::JsonConfigParser::get_value_or(json const& config, std::string const& name, T value)
{
   const auto found = config.find(name);
   if (found != config.end()) {
      value = found.value();
   }
   return value;
}
std::vector<variable> JsonConfigParser::get_variables(json const& config)
{
   std::vector<variable> variables;
   const auto& jvariables = get_value_or(config, TAG_VARIABLES, json{});
   std::ranges::for_each(jvariables.items(), [&variables](const auto& item) {
      variables.emplace_back(variable{item.value().at(TAG_STARTS_WITH), item.value().at(TAG_ENDS_WITH)});
   });
   return variables;
}

std::vector<pair> JsonConfigParser::get_pair(json const& config, translation& tr)
{
   std::vector<pair> pairs;
   const auto& jpairswith = get_value_or(config, TAG_PAIRSWITH, json{});
   std::ranges::for_each(jpairswith.items(), [&pairs, this, &tr](const auto& item) {
      pair new_pair;
      const json& jpairswithitem = item.value();
      try {
         new_pair.print_match = jpairswithitem.at(TAG_PRINTMATCH).get<std::vector<std::string>>();
      }
      catch (...) {
         new_pair.print_match.push_back(tr.print);
      }
      try {
         new_pair.before_match = jpairswithitem.at(TAG_BEFORE).get<std::vector<std::string>>();
      }
      catch (...) {
         new_pair.before_match.push_back(tr.print);
      }
      new_pair.pair_match = jpairswithitem.at(TAG_PAIRMATCH).get<std::vector<std::string>>();
      new_pair.error_print = get_value_or(jpairswithitem, TAG_ERROR_PRINT, std::string{});
      pairs.emplace_back(new_pair);
   });
   return pairs;
}

std::vector<translation> JsonConfigParser::load_translations(json const& config, std::string const& name)
{
   std::vector<translation> translations;

   for (const auto& item : config.at(name).items()) {
      // item {tranlation_element1, tranlation_element2, ...}

      const json& jtranslation = item.value();
      const std::string category = get_value_or(jtranslation, TAG_CATEGORY, std::string{});
      bool enable = get_value_or(jtranslation, TAG_ENABLE, true);

      if (!enable || is_disabled(category)) {
         continue;
      }

      translation tr;
      tr.category = category;

      try {
         tr.patterns = jtranslation.at(TAG_PATTERNS).get<std::vector<std::string>>();
      }
      catch (...) {
         std::cerr << "[warn] patterns not defined\n";
         continue;
      }
      if (tr.patterns.empty()) {
         std::cerr << "[warn] patterns empty\n";
         continue;
      }

      tr.print = get_value_or(jtranslation, TAG_PRINT, std::string{});
      if (tr.print.empty()) {
         std::cerr << "[warn] print not defined or empty\n";
         continue;
      }

      tr.variables = get_variables(jtranslation);

      auto new_pairs = get_pair(jtranslation, tr);
      push_pairs(new_pairs);

      std::string dup = get_value_or(jtranslation, TAG_DUPLICATES, std::string{});
      tr.duplicates = get_duplicate_type(dup);

      translations.push_back(std::move(tr));
   }
   return translations;
}

std::vector<translation> JsonConfigParser::load_translations_csv(std::string const& translations_csv_file)
{
   std::vector<translation> translations;
   std::filesystem::path p(config_file_);
   std::string csv_file = (p.parent_path() / std::filesystem::path(translations_csv_file)).string();

   const short no_of_columns = 13;
   io::CSVReader<no_of_columns, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>> in(csv_file);
   in.read_header(io::ignore_extra_column, "enable", "group", "print", "duplicates", "pattern1", "pattern2", "pattern3",
                  "variable1_starts_with", "variable1_ends_with", "variable2_starts_with", "variable2_ends_with",
                  "variable3_starts_with", "variable3_ends_with");
   std::string enable;
   std::string group;
   std::string print;
   std::string duplicates;
   std::string pattern1;
   std::string pattern2;
   std::string pattern3;
   std::string v1s;
   std::string v1e;
   std::string v2s;
   std::string v2e;
   std::string v3s;
   std::string v3e;
   while (in.read_row(enable, group, print, duplicates, pattern1, pattern2, pattern3, v1s, v1e, v2s, v2e, v3s, v3e)) {
      if (enable == "No" || enable == "no" || enable == "False" || enable == "false" || enable == "0") {
         continue;
      }
      if (is_disabled(group)) {
         continue;
      }
      if (pattern1.empty() && pattern2.empty() && pattern3.empty()) {
         std::cerr << "[warn] patterns empty\n";
         continue;
      }
      translation tr;
      tr.category = group;
      tr.print = print;
      tr.duplicates = get_duplicate_type(duplicates);
      if (!pattern1.empty()) {
         tr.patterns.push_back(pattern1);
      }
      if (!pattern2.empty()) {
         tr.patterns.push_back(pattern2);
      }
      if (!pattern3.empty()) {
         tr.patterns.push_back(pattern3);
      }
      if (!v1s.empty()) {
         tr.variables.push_back(variable{v1s, v1e});
      }
      if (!v2s.empty()) {
         tr.variables.push_back(variable{v2s, v2e});
      }
      if (!v3s.empty()) {
         tr.variables.push_back(variable{v3s, v3e});
      }
      translations.push_back(tr);
   }
   return translations;
}

JsonConfigParser::JsonConfigParser(std::string config_file) : config_file_{std::move(config_file)}
{
   if (config_file_.empty()) {
      config_file_ = "config.json";
   }
}

JsonConfigParser::JsonConfigParser(nlohmann::json config) : config_(std::move(config))
{
}

void JsonConfigParser::read_config_file()
{
   std::ifstream file(config_file_);
   file >> config_;
   std::cout << "configuration loaded from " << config_file_ << '\n';
}

void JsonConfigParser::load_disabled_categories()
{
   set_disabled_categories(config_.at(TAG_DISABLE_CATEGORY).get<std::vector<std::string>>());
}

void JsonConfigParser::load_translations()
{
   std::string translations_csv_file;
   try {
      translations_csv_file = config_.at(TAG_TRANSLATIONS_CSV).get<std::string>();
   }
   catch (...) {
      translations_csv_file = "";
   }
   if (!translations_csv_file.empty()) {
      set_translations(load_translations_csv(translations_csv_file));
      try {
         config_.at(TAG_TRANSLATIONS);
         std::cerr << "[warn] " << TAG_TRANSLATIONS << " is not read in the presence of " << TAG_TRANSLATIONS_CSV
                   << "\n";
      }
      catch (...) {
      }
   }
   else {
      set_translations(load_translations(config_, TAG_TRANSLATIONS));
   }
}

void JsonConfigParser::load_wrap_text()
{
   try {
      set_wrap_text_pre(config_.at(TAG_WRAPTEXT_PRE).get<std::vector<std::string>>());
   }
   catch (...) {
   }
   try {
      set_wrap_text_post(config_.at(TAG_WRAPTEXT_POST).get<std::vector<std::string>>());
   }
   catch (...) {
   }
}

void JsonConfigParser::load_blacklists()
{
   try {
      // blacklists_ = std::vector<std::string>(config_.at(TAG_BLACKLIST));
      set_blacklists(config_.at(TAG_BLACKLIST).get<std::vector<std::string>>());
   }
   catch (...) {
   }
}

void JsonConfigParser::load_delete_lines()
{
   auto deletors = config_.at(TAG_DELETE_LINES).get<std::vector<std::string>>();

   std::vector<std::regex> delete_lines_regex;
   std::vector<std::string> delete_lines;
   for (auto const& entry : deletors) {
      if (entry.find_first_of("[\\^$.|?*+") != std::string::npos) {
         delete_lines_regex.emplace_back(
             entry, std::regex_constants::grep | std::regex_constants::nosubs | std::regex_constants::optimize);
      }
      else {
         delete_lines.emplace_back(entry);
      }
   }
   set_delete_lines_regex(delete_lines_regex);
   set_delete_lines(delete_lines);

   if (!delete_lines_regex.empty()) {
      std::cerr << "[warn] Use of regex in " << TAG_DELETE_LINES << " is a lot slower. Use normal search instead,\n";
      for (auto const& entry : deletors) {
         if (entry.find_first_of("[\\^$.|?*+") != std::string::npos) {
            std::cout << "  " << entry << '\n';
         }
      }
   }
}

void JsonConfigParser::load_replace_words()
{
   const json j_tr = config_.at(TAG_REPLACE_WORDS);
   std::vector<replacement> replace_words;
   for (const auto& [key, value] : j_tr.items()) {
      replace_words.emplace_back(key, value);
   }
   set_replace_words(replace_words);
}

void JsonConfigParser::load_execute()
{
   set_execute_commands(config_.at(TAG_EXECUTE).get<std::vector<std::string>>());
}

void JsonConfigParser::load_translation_file()
{
   set_translation_file(config_.at(TAG_TRANSLATION_FILE));
}

void JsonConfigParser::load_backup_file()
{
   set_backup_file(config_.at(TAG_BACKUP_FILE));
}

void JsonConfigParser::load_auto_new_line()
{
   set_auto_new_line(config_.at(TAG_AUTO_NEW_LINE));
}

}  // namespace Logalizer::Config

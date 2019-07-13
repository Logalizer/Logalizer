#include "jsonconfigparser.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace Logalizer::Config {

using json = nlohmann::json;

namespace details {

template <class T>
T getValue(json const &config, std::string const &name)
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

std::vector<variable> getVariables(json const &jvariables)
{
   std::vector<variable> variables;
   for (auto &[key, value] : jvariables.items()) {
      variables.emplace_back(variable{value["startswith"], value["endswith"]});
   }
   return variables;
}

std::vector<std::string> loadArray(json const &config, std::string const &name)
{
   std::vector<std::string> array;
   json j_bl = getValue<json>(config, name);

   for (auto &[key, value] : j_bl.items()) {
      array.push_back(value);
   }
   return array;
}

std::vector<translation> loadTranslations(json const &config, std::string const &name,
                                          std::vector<std::string> disabled_categories)
{
   std::vector<translation> translations;
   translation tr;
   json j_tr = getValue<json>(config, name);

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
      tr.patterns = loadArray(entry, "patterns");
      tr.print = entry["print"];
      tr.variables = getVariables(entry["variables"]);
      translations.push_back(std::move(tr));
   }
   return translations;
}

std::vector<translation> loadTranslations_emplace(json const &config, std::string const &name,
                                                  std::vector<std::string> disabled_categories)
{
   std::vector<translation> translations;

   json j_tr = getValue<json>(config, name);

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

      translations.emplace_back(entry["category"], loadArray(entry, "patterns"), entry["print"],
                                getVariables(entry["variables"]));
   }
   return translations;
}
}  // namespace details

JsonConfigParser::JsonConfigParser(std::string const &config_file) : ConfigParser(config_file)
{
   if (config_file_.empty()) config_file_ = "config.json";
}

void JsonConfigParser::loadConfigFile()
{
   std::ifstream file(config_file_);
   file >> config_;
   std::cout << "configuration loaded from " << config_file_ << '\n';
}

void JsonConfigParser::loadAllConfigurations()
{
   loadTranslations();
   loadWrapText();
   loadBlacklists();
   loadDeleteLines();
   loadReplaceWords();
   loadGenerateUML();
   loadUMLFile();
   loadBackupFile();
}

JsonConfigParser::~JsonConfigParser()
{
}

void JsonConfigParser::loadTranslations()
{
   disabled_categories_ = details::loadArray(config_, TAG_DISABLE_CATEGORY);
   translations_ = details::loadTranslations_emplace(config_, TAG_TRANSLATIONS, disabled_categories_);
}

void JsonConfigParser::loadWrapText()
{
   wrap_text_pre_ = details::loadArray(config_, TAG_WRAPTEXT_PRE);
   wrap_text_post_ = details::loadArray(config_, TAG_WRAPTEXT_POST);
}

void JsonConfigParser::loadBlacklists()
{
   blacklists_ = details::loadArray(config_, TAG_BLACKLIST);
}

void JsonConfigParser::loadDeleteLines()
{
   std::vector<std::string> deletors = details::loadArray(config_, TAG_DELETE_LINES);

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
void JsonConfigParser::loadReplaceWords()
{
   json j_tr = details::getValue<json>(config_, TAG_REPLACE_WORDS);
   for (auto &[key, value] : j_tr.items()) {
      replace_words_.emplace_back(key, value);
   }
}

void JsonConfigParser::loadGenerateUML()
{
   std::vector<std::string> lines = details::loadArray(config_, TAG_GENERATE_UML);
   std::stringstream ss;
   copy(begin(lines), end(lines), std::ostream_iterator<std::string>(ss, " "));
   generate_uml_command_ = ss.str();
}

void JsonConfigParser::loadUMLFile()
{
   uml_file_ = details::getValue<std::string>(config_, TAG_TRANSLATED_UML_FILE);
}

void JsonConfigParser::loadBackupFile()
{
   backup_file_ = details::getValue<std::string>(config_, TAG_BACKUP_FILE);
}

}  // namespace Logalizer::Config

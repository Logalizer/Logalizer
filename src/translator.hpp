#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include "jsonconfigparser.h"
#include "path_variable_utils.h"

using namespace Logalizer::Config;

std::string getVariableValues_r(std::string const &line, std::vector<variable> const &variables)
{
   std::string value;
   if (variables.size() == 0) return value;

   value = "(";

   for (auto const &v : variables) {
      std::regex regex(v.startswith + "(.*)" + v.endswith);
      std::smatch match;
      regex_search(line, match, regex);
      if (match.size() == 2) {
         value += match[1].str() + ", ";
      }
   }

   if (value.back() == ' ') {
      value.erase(value.end() - 2, value.end());
   }

   value += ")";
   return value;
}

std::string getVariableValues_b(std::string const &line, std::vector<variable> const &variables)
{
   std::string value;
   if (variables.size() == 0) return value;

   value = "(";

   for (auto const &v : variables) {
      auto start_point = line.find(v.startswith);
      if (start_point == std::string::npos) continue;

      start_point += v.startswith.size();
      auto end_point = line.find(v.endswith, start_point);
      if (end_point == std::string::npos) continue;

      std::string capture(line.begin() + static_cast<long>(start_point), line.begin() + static_cast<long>(end_point));

      value += capture + ", ";
   }

   if (value.back() == ' ') {
      value.erase(value.end() - 2, value.end());
   }

   value += ")";
   return value;
}

std::vector<std::string> getVariableValues(std::string const &line, std::vector<variable> const &variables)
{
   std::vector<std::string> value;
   if (variables.size() == 0) return {};

   for (auto const &v : variables) {
      auto start_point = line.find(v.startswith);
      if (start_point == std::string::npos) continue;

      start_point += v.startswith.size();
      auto end_point = line.find(v.endswith, start_point);
      if (end_point == std::string::npos) continue;

      std::string capture(line.begin() + static_cast<long>(start_point), line.begin() + static_cast<long>(end_point));

      value.push_back(std::move(capture));
   }

   return value;
}

void getReplacedLine(std::string const &line, std::vector<variable> const &variables, std::string *replacement)
{
   std::vector<std::string> v = getVariableValues(line, variables);

   bool formatted_print = false;
   if (replacement->find("${1}") != std::string::npos) {
      formatted_print = true;
   }

   if (formatted_print) {
      int i = 1;
      for (auto entry : v) {
         std::string token = "${" + std::to_string(i++) + "}";
         Logalizer::Config::Utils::findAndReplace(replacement, token, entry);
      }
   }
   else if (v.size()) {
      std::string params = "(";
      for (auto entry : v) {
         params += entry + ", ";
      }
      if (params.back() == ' ') {
         params.erase(params.end() - 2, params.end());
      }

      params += ")";
      replacement->append(params);
   }
}

bool matchTranslation(std::string const &line, ConfigParser const *data, std::string &translation)
{
   auto found = std::find_if(cbegin(data->getTranslations()), cend(data->getTranslations()),
                             [&line](auto const &tr) { return tr.in(line); });
   if (found != cend(data->getTranslations())) {
      bool blacklisted = std::any_of(cbegin(data->getBlacklists()), cend(data->getBlacklists()),
                                     [&line](auto const &bl) { return line.find(bl) != std::string::npos; });
      if (!blacklisted) {
         translation = found->print;
         getReplacedLine(line, found->variables, &translation);
         return true;
      }
   }

   return false;
}

[[nodiscard]] bool isLineDeleted(std::string const &line, std::vector<std::string> const &delete_lines,
                                 std::vector<std::regex> const &delete_lines_regex) noexcept
{
   bool deleted = std::any_of(cbegin(delete_lines), cend(delete_lines),
                              [&line](auto const &dl) { return line.find(dl) != std::string::npos; });

   if (deleted) return true;

   deleted = std::any_of(cbegin(delete_lines_regex), cend(delete_lines_regex),
                         [&line](auto const &dl) { return regex_search(line, dl); });

   return deleted;
}

void replaceWords(std::string *line, std::vector<replacement> const &replacemnets)
{
   for (auto const &entry : replacemnets) Utils::findAndReplace(line, entry.search, entry.replace);
}

void createTranslationFile(std::string const &trace_file_name, std::string const &translation_file_name,
                           ConfigParser const *data)
{
   std::ifstream trace_file(trace_file_name);
   std::string trim_file_name = trace_file_name + ".trim.log";
   std::ofstream trimmed_file(trim_file_name);
   std::ofstream translation_file(translation_file_name);
   for (auto const &line : data->getWrapTextPre()) {
      translation_file << line << '\n';
   }

   for (std::string line; getline(trace_file, line);) {
      if (isLineDeleted(line, data->getDeleteLines(), data->getDeleteLinesRegex())) {
         continue;
      }

      replaceWords(&line, data->getReplaceWords());

      trimmed_file << line << '\n';

      if (std::string translation; matchTranslation(line, data, translation)) {
         translation_file << translation << '\n';
      }
   }

   for (auto const &line : data->getWrapTextPost()) {
      translation_file << line << '\n';
   }

   translation_file.close();
   trimmed_file.close();
   trace_file.close();
   remove(trace_file_name.c_str());
   rename(trim_file_name.c_str(), trace_file_name.c_str());
}

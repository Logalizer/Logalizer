#pragma once
#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include "jsonconfigparser.h"
#include "path_variable_utils.h"

using namespace Logalizer::Config;

std::string fetch_values_regex(std::string const &line, std::vector<variable> const &variables)
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

std::string fetch_values_braced(std::string const &line, std::vector<variable> const &variables)
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

std::string capture_values(variable const &var, std::string const &content)
{
   auto start_point = content.find(var.startswith);
   if (start_point == std::string::npos) return " ";

   start_point += var.startswith.size();
   auto end_point = content.find(var.endswith, start_point);
   if (end_point == std::string::npos) return " ";

   std::string capture(content.begin() + static_cast<long>(start_point),
                       content.begin() + static_cast<long>(end_point));

   return capture;
}

std::vector<std::string> fetch_values(std::string const &line, std::vector<variable> const &variables)
{
   std::vector<std::string> value;
   if (variables.size() == 0) return {};

   std::transform(begin(variables), end(variables), std::back_inserter(value),
                  std::bind(capture_values, std::placeholders::_1, line));

   return value;
}

std::string pack_parameters(std::vector<std::string> const &v)
{
   auto comma_fold = [](std::string a, std::string b) { return a + ", " + b; };
   std::string initial_value = "(" + v[0];
   std::string params = std::accumulate(next(begin(v)), end(v), initial_value, comma_fold) + ")";
   return params;
}

std::string fill_values_formatted(std::vector<std::string> const &values, std::string const &line_to_fill)
{
   std::string filled_line = line_to_fill;
   for (size_t i = 1; i <= values.size(); ++i) {
      std::string token = "${" + std::to_string(i) + "}";
      Utils::replace_all(&filled_line, token, values[i]);
   }
   return filled_line;
}

std::string fill_values(std::vector<std::string> const &values, std::string const &line_to_fill)
{
   std::string filled_line;
   bool formatted_print = (line_to_fill.find("${1}") != std::string::npos);
   if (formatted_print) {
      filled_line = fill_values_formatted(values, line_to_fill);
   }
   else if (values.size()) {
      filled_line = line_to_fill + pack_parameters(values);
   }
   return filled_line;
}

[[nodiscard]] bool is_blacklisted(std::string const &line, std::vector<std::string> const &blacklists)
{
   return std::any_of(cbegin(blacklists), cend(blacklists),
                      [&line](auto const &bl) { return line.find(bl) != std::string::npos; });
}

auto match(std::string const &line, std::vector<translation> const &translations,
           std::vector<std::string> const &blacklists)
{
   auto found = std::find_if(cbegin(translations), cend(translations), [&line](auto const &tr) { return tr.in(line); });
   if (found != cend(translations)) {
      if (!is_blacklisted(line, blacklists)) {
         return found;
      }
   }

   return found;
}

[[nodiscard]] bool is_deleted(std::string const &line, std::vector<std::string> const &delete_lines,
                              std::vector<std::regex> const &delete_lines_regex) noexcept
{
   bool deleted = std::any_of(cbegin(delete_lines), cend(delete_lines),
                              [&line](auto const &dl) { return line.find(dl) != std::string::npos; });

   if (deleted) return true;

   deleted = std::any_of(cbegin(delete_lines_regex), cend(delete_lines_regex),
                         [&line](auto const &dl) { return regex_search(line, dl); });

   return deleted;
}

void replace(std::string *line, std::vector<replacement> const &replacemnets)
{
   std::for_each(cbegin(replacemnets), cend(replacemnets),
                 [&](auto const &entry) { Utils::replace_all(line, entry.search, entry.replace); });
}

void translate_file(std::string const &trace_file_name, std::string const &translation_file_name,
                    ConfigParser const *parser, std::future<void> &backup_future)
{
   std::ifstream trace_file(trace_file_name);
   std::string trim_file_name = trace_file_name + ".trim.log";
   std::ofstream trimmed_file(trim_file_name);
   std::vector<std::string> translations;

   auto pre_text = parser->get_wrap_text_pre();
   std::copy(pre_text.begin(), pre_text.end(), std::back_inserter(translations));

   for (std::string line; getline(trace_file, line);) {
      if (is_deleted(line, parser->get_delete_lines(), parser->get_delete_lines_regex())) {
         continue;
      }

      replace(&line, parser->get_replace_words());
      trimmed_file << line << '\n';

      auto found = match(line, parser->get_translations(), parser->get_blacklists());
      if (found != cend(parser->get_translations())) {
         bool repeat_allowed = found->repeat;
         std::vector<std::string> values = fetch_values(line, found->variables);
         std::string translation = fill_values(values, found->print);
         bool new_entry = std::none_of(cbegin(translations), cend(translations),
                                       [&translation](auto const &entry) { return entry == translation; });
         if (repeat_allowed || new_entry) {
            translations.emplace_back(translation);
         }
      }
   }
   auto post_text = parser->get_wrap_text_post();
   std::copy(post_text.begin(), post_text.end(), std::back_inserter(translations));

   Utils::mkdir(Utils::dir_file(translation_file_name).first);

   std::ofstream translation_file(translation_file_name);
   std::copy(translations.begin(), translations.end(), std::ostream_iterator<std::string>(translation_file, "\n"));
   translation_file.close();

   trimmed_file.close();
   trace_file.close();
   backup_future.wait();
   remove(trace_file_name.c_str());
   rename(trim_file_name.c_str(), trace_file_name.c_str());
}

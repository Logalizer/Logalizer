#include "translator.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include "config_types.h"

namespace fs = std::filesystem;
using namespace Logalizer::Config;

std::string Translator::fetch_values_regex(std::string const &line, std::vector<variable> const &variables)
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

std::string Translator::fetch_values_braced(std::string const &line, std::vector<variable> const &variables)
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

      std::string capture(line.cbegin() + static_cast<long>(start_point), line.cbegin() + static_cast<long>(end_point));

      value += capture + ", ";
   }

   if (value.back() == ' ') {
      value.erase(value.end() - 2, value.end());
   }

   value += ")";
   return value;
}

std::string Translator::capture_values(variable const &var, std::string const &content)
{
   auto start_point = content.find(var.startswith);
   if (start_point == std::string::npos) return " ";

   start_point += var.startswith.size();
   const auto end_point = content.find(var.endswith, start_point);
   if (end_point == std::string::npos || var.endswith.empty()) {
      // if endswith is not matching or empty, capture till the end
      std::string capture(content.cbegin() + static_cast<long>(start_point), content.cend());
      return capture;
   }

   std::string capture(content.cbegin() + static_cast<long>(start_point),
                       content.cbegin() + static_cast<long>(end_point));

   return capture;
}

std::vector<std::string> Translator::variable_values(std::string const &line, std::vector<variable> const &variables)
{
   std::vector<std::string> value;
   if (variables.size() == 0) return {};

   std::transform(cbegin(variables), cend(variables), std::back_inserter(value),
                  [&line, this](auto const &var) { return capture_values(var, line); });

   return value;
}

std::string Translator::pack_parameters(std::vector<std::string> const &v)
{
   auto comma_fold = [](std::string a, std::string b) { return a + ", " + b; };
   const std::string initial_value = "(" + v[0];
   std::string params = std::accumulate(next(cbegin(v)), cend(v), initial_value, comma_fold) + ")";
   return params;
}

std::string Translator::fill_values_formatted(std::vector<std::string> const &values, std::string const &line_to_fill)
{
   std::string filled_line = line_to_fill;
   for (size_t i = 1, len = values.size(); i <= len; ++i) {
      const std::string token = "\\$\\{" + std::to_string(i) + "\\}";
      filled_line = std::regex_replace(filled_line, std::regex(token), values[i - 1]);
   }
   return filled_line;
}

std::string Translator::update_variables(std::vector<std::string> const &values, std::string const &line_to_fill)
{
   std::string filled_line;
   const bool formatted_print = (line_to_fill.find("${1}") != std::string::npos);
   if (formatted_print) {
      filled_line = fill_values_formatted(values, line_to_fill);
   }
   else if (values.size()) {
      filled_line = line_to_fill + pack_parameters(values);
   }
   else {
      filled_line = line_to_fill;
   }
   return filled_line;
}

[[nodiscard]] bool Translator::is_blacklisted(std::string const &line)
{
   std::vector<std::string> const &blacklists = config_.get_blacklists();
   return std::any_of(cbegin(blacklists), cend(blacklists),
                      [&line](auto const &bl) { return line.find(bl) != std::string::npos; });
}

auto Translator::get_matching_translator(std::string const &line)
{
   const std::vector<translation> &trcfg = config_.get_translations();
   auto found = std::find_if(cbegin(trcfg), cend(trcfg), [&line](auto const &tr) { return tr.in(line); });
   if (found != cend(trcfg)) {
      if (!is_blacklisted(line)) {
         return found;
      }
   }
   return cend(trcfg);
}

[[nodiscard]] bool Translator::is_deleted(std::string const &line) noexcept
{
   std::vector<std::string> const &delete_lines = config_.get_delete_lines();
   std::vector<std::regex> const &delete_lines_regex = config_.get_delete_lines_regex();
   bool deleted = std::any_of(cbegin(delete_lines), cend(delete_lines),
                              [&line](auto const &dl) { return line.find(dl) != std::string::npos; });

   if (deleted) return true;

   deleted = std::any_of(cbegin(delete_lines_regex), cend(delete_lines_regex),
                         [&line](auto const &dl) { return regex_search(line, dl); });

   return deleted;
}

void Translator::replace_words(std::string *line)
{
   std::vector<replacement> const &replacements = config_.get_replace_words();
   std::for_each(cbegin(replacements), cend(replacements), [&](auto const &entry) {
      *line = std::regex_replace(*line, std::regex(entry.search), entry.replace);
   });
}

void Translator::add_translation(std::string &&translation, duplicates_t duplicates)
{
   auto contains = [this](const auto &str) { return std::find(cbegin(translations), cend(translations), str); };

   switch (duplicates) {
      case duplicates_t::allowed: {
         translations.emplace_back(std::move(translation));
         break;
      }
      case duplicates_t::remove: {
         if (contains(translation) == translations.cend()) translations.emplace_back(std::move(translation));
         break;
      }
      case duplicates_t::remove_continuous: {
         if (translations.empty() || translation != translations.back())
            translations.emplace_back(std::move(translation));
         break;
      }
      case duplicates_t::count: {
         auto first = contains(translation);
         if (translations.empty() || first == translations.cend()) {
            translations.emplace_back(std::move(translation));
            trans_count[translations.size() - 1]++;
         }
         else {
            trans_count[static_cast<size_t>(std::distance(cbegin(translations), first))]++;
         }
         break;
      }
      case duplicates_t::count_continuous: {
         if (translations.empty() || translation != translations.back()) {
            translations.emplace_back(std::move(translation));
         }
         trans_count[translations.size() - 1]++;
         break;
      }
   }
}

void Translator::update_count()
{
   for (auto const &[index, count] : trans_count) {
      translations[index] =
          std::regex_replace(translations[index], std::regex("\\$\\{count\\}"), std::to_string(count));
   }
}

void Translator::add_pre_text()
{
   const auto &pre_text = config_.get_wrap_text_pre();
   std::copy(pre_text.cbegin(), pre_text.cend(), std::back_inserter(translations));
}

void Translator::add_post_text()
{
   const auto &post_text = config_.get_wrap_text_post();
   std::copy(post_text.cbegin(), post_text.cend(), std::back_inserter(translations));
}

void Translator::write_translation_file()
{
   std::string const &tr_file_name = config_.get_translation_file();
   fs::create_directories(fs::path(tr_file_name).remove_filename());
   std::ofstream translation_file(tr_file_name);
   if (config_.get_auto_new_line()) {
      std::copy(translations.cbegin(), translations.cend(), std::ostream_iterator<std::string>(translation_file, "\n"));
   }
   else {
      std::copy(translations.cbegin(), translations.cend(), std::ostream_iterator<std::string>(translation_file));
   }
   translation_file.close();
}

void Translator::write_to_file(std::string const &line, std::ofstream &trimmed_file)
{
   trimmed_file << line << '\n';
}

void Translator::translate(std::string const &line)
{
   const auto &trcfg = get_matching_translator(line);
   if (trcfg != cend(config_.get_translations())) {
      std::string translation = update_variables(variable_values(line, trcfg->variables), trcfg->print);
      add_translation(std::move(translation), trcfg->duplicates);
   }
}

void Translator::translate_file(std::string const &trace_file_name)
{
   std::ifstream trace_file(trace_file_name);
   const std::string trim_file_name = trace_file_name + ".trim.log";
   std::ofstream trimmed_file(trim_file_name);
   add_pre_text();
   for (std::string line; getline(trace_file, line);) {
      if (is_deleted(line)) continue;

      replace_words(&line);
      write_to_file(line, trimmed_file);
      translate(line);
   }
   update_count();
   add_post_text();
   write_translation_file();
   translations.clear();
   trimmed_file.close();
   trace_file.close();
   remove(trace_file_name.c_str());
   rename(trim_file_name.c_str(), trace_file_name.c_str());
}

void Translator::execute_commands()
{
   for (auto const &command : config_.get_execute_commands()) {
      std::cout << "Executing...\n";
      const char *command_str = command.c_str();
      std::cout << command_str << std::endl;
      if (const int returnval = system(command_str)) {
         std::cerr << TAG_EXECUTE << " : " << command << " execution failed with code " << returnval << "\n";
         break;
      }
   }
}

#include "translator.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <ranges>
#include <regex>
#include "config_types.h"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;
using namespace Logalizer::Config;
namespace rgs = std::ranges;

#if 0
std::string Translator::fetch_values_regex(std::string const& line, std::vector<variable> const& variables)
{
   std::string value;
   if (variables.empty()) {
      return value;
   }

   value = "(";

   for (auto const& v : variables) {
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

std::string Translator::fetch_values_braced(std::string const& line, std::vector<variable> const& variables)
{
   std::string value;
   if (variables.empty()) {
      return value;
   }

   value = "(";

   for (auto const& v : variables) {
      auto start_point = line.find(v.startswith);
      if (start_point == std::string::npos) {
         continue;
      }

      start_point += v.startswith.size();
      auto end_point = line.find(v.endswith, start_point);
      if (end_point == std::string::npos) {
         continue;
      }

      std::string capture(line.cbegin() + static_cast<long>(start_point), line.cbegin() + static_cast<long>(end_point));

      value += capture + ", ";
   }

   if (value.back() == ' ') {
      value.erase(value.end() - 2, value.end());
   }

   value += ")";
   return value;
}
#endif

std::string Translator::capture_values(variable const& var, std::string const& content)
{
   auto start_point = content.find(var.startswith);
   if (start_point == std::string::npos) {
      return " ";
   }

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

std::vector<std::string> Translator::variable_values(std::string const& line, std::vector<variable> const& variables)
{
   std::vector<std::string> value;
   if (variables.empty()) {
      return {};
   }

   rgs::transform(variables, std::back_inserter(value),
                  [&line, this](auto const& var) { return capture_values(var, line); });

   return value;
}

std::string Translator::pack_parameters(std::vector<std::string> const& v)
{
   auto comma_fold = [](const std::string& a, const std::string& b) { return a + ", " + b; };
   const std::string initial_value = "(" + v[0];
   std::string params = std::accumulate(next(cbegin(v)), cend(v), initial_value, comma_fold) + ")";
   return params;
}

std::string Translator::fill_values_formatted(std::vector<std::string> const& values, std::string const& line_to_fill)
{
   std::string filled_line = line_to_fill;
   for (size_t i = 1, len = values.size(); i <= len; ++i) {
      const std::string token = "\\$\\{" + std::to_string(i) + "\\}";
      filled_line = std::regex_replace(filled_line, std::regex(token), values[i - 1]);
   }
   return filled_line;
}

std::string Translator::update_variables(std::vector<std::string> const& values, std::string const& line_to_fill)
{
   std::string filled_line;
   if (values.empty()) {
      filled_line = line_to_fill;
   }
   else {
      const bool formatted_print = (line_to_fill.find("${1}") != std::string::npos);
      if (formatted_print) {
         filled_line = fill_values_formatted(values, line_to_fill);
      }
      else {
         filled_line = line_to_fill + pack_parameters(values);
      }
   }
   return filled_line;
}

[[nodiscard]] bool Translator::is_blacklisted(std::string const& line)
{
   return rgs::any_of(config_.get_blacklists(), [&line](auto const& bl) { return line.find(bl) != std::string::npos; });
}

auto Translator::get_matching_translator(std::string const& line)
{
   auto matcher = [&line](auto const& tr) { return tr.in(line); };
   const std::vector<translation>& trcfg = config_.get_translations();
   auto found = rgs::find_if(trcfg, matcher);
   // auto found = rgs::find_if(trcfg, std::bind(std::mem_fn(&translation::in), std::placeholders::_1, line));
   if (found != cend(trcfg)) {
      if (!is_blacklisted(line)) {
         return found;
      }
   }
   return cend(trcfg);
}

[[nodiscard]] bool Translator::is_deleted(std::string const& line) noexcept
{
   bool deleted =
       rgs::any_of(config_.get_delete_lines(), [&line](auto const& dl) { return line.find(dl) != std::string::npos; });

   if (deleted) {
      return true;
   }

   deleted = rgs::any_of(config_.get_delete_lines_regex(), [&line](auto const& dl) { return regex_search(line, dl); });

   return deleted;
}

void Translator::replace_words(std::string* line)
{
   auto regex_replace = [&](auto const& entry) {
      *line = std::regex_replace(*line, std::regex(entry.search), entry.replace);
   };
   rgs::for_each(config_.get_replace_words(), regex_replace);
}

void Translator::add_translation(std::string&& translation, duplicates_t duplicates)
{
   auto contains = [this](const auto& str) { return std::find(cbegin(translations), cend(translations), str); };

   spdlog::debug("Adding translation {}", translation);

   switch (duplicates) {
      case duplicates_t::allowed: {
         translations.emplace_back(std::move(translation));
         break;
      }
      case duplicates_t::remove: {
         if (contains(translation) == translations.cend()) {
            translations.emplace_back(std::move(translation));
         }
         break;
      }
      case duplicates_t::remove_continuous: {
         if (translations.empty() || translation != translations.back()) {
            translations.emplace_back(std::move(translation));
         }
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
   for (auto const& [index, count] : trans_count) {
      translations[index] =
          std::regex_replace(translations[index], std::regex(R"(\$\{count\})"), std::to_string(count));
   }
}

void Translator::add_pre_text()
{
   rgs::copy(config_.get_wrap_text_pre(), std::back_inserter(translations));
}

void Translator::add_post_text()
{
   rgs::copy(config_.get_wrap_text_post(), std::back_inserter(translations));
}

void Translator::write_translation_file()
{
   std::string const& tr_file_name = config_.get_translation_file();
   fs::create_directories(fs::path(tr_file_name).remove_filename());
   std::ofstream translation_file(tr_file_name);
   if (config_.get_auto_new_line()) {
      rgs::copy(translations, std::ostream_iterator<std::string>(translation_file, "\n"));
   }
   else {
      rgs::copy(translations, std::ostream_iterator<std::string>(translation_file));
   }
   translation_file.close();
}

void Translator::write_to_file(std::string const& line, std::ofstream& trimmed_file)
{
   trimmed_file << line << '\n';
}

std::string Translator::fill_values(std::string const& line, Logalizer::Config::translation const& tr)
{
   std::string updated_print;
   auto values = variable_values(line, tr.variables);
   updated_print = update_variables(values, tr.print);
   return updated_print;
   // std::string translation = update_variables(variable_values(line, trcfg->variables), trcfg->print);
   // std::string translation = line
   //    | variable_values(trcfg->variables)
   //    | update_variables(trcfg->print);
   // flow(
   //    variable_values(trcfg->variables),
   //    update_variables(trcfg->print)
   // )(line);
}

void Translator::translate(std::string const& line)
{
   const auto& trcfg = get_matching_translator(line);
   if (trcfg == cend(config_.get_translations())) {
      return;
   }
   const auto& tr = *trcfg;
   std::string translation = fill_values(line, tr);
   add_translation(std::move(translation), trcfg->duplicates);
}

bool Translator::matches_pattern(std::string const& line, std::vector<std::string>& patterns) const
{
   auto matches = [&line](auto const& pattern) { return static_cast<bool>(line.find(pattern) != std::string::npos); };
   return std::all_of(cbegin(patterns), cend(patterns), matches);
}

bool Translator::matches_pattern(std::string const& line, std::string& pattern) const
{
   return static_cast<bool>(line.find(pattern) != std::string::npos);
}

void Translator::validate_pairs()
{
   std::vector<std::pair<int, std::string>> insertions;
   for (const auto& pair : config_.get_pairs()) {
      size_t source = INT32_MAX;
      size_t pairswith = INT32_MAX;
      size_t before = INT32_MAX;
      for (size_t i = 0; i < translations.size(); ++i) {
         const std::string& line = translations[i];
         // case 5: Source is already found and source is found again
         if (source != INT32_MAX && line.find(pair.source) != std::string::npos) {
            insertions.push_back({i, pair.error});
            source = i;
            continue;
         }
         // case 1: Source is found
         else if (line.find(pair.source) != std::string::npos) {
            source = i;
            continue;
         }
         // case 2: Source is found, matching pair is found
         else if (source != INT32_MAX && line.find(pair.pairswith) != std::string::npos) {
            source = pairswith = INT32_MAX;
            continue;
         }
         // case 3: Source is found, before is found before a matching pair is found
         else if (source != INT32_MAX && line.find(pair.before) != std::string::npos) {
            insertions.push_back({i, pair.error});
            source = before = INT32_MAX;
         }
      }
      // case 4: Source is found and pairswith is not found till EOF
      if (source != INT32_MAX && pairswith == INT32_MAX && before == INT32_MAX) {
         translations.push_back(pair.error);
      }
   }
   // Insert all the errors
   for (const auto& item : insertions) {
      translations.insert(translations.begin() + item.first, item.second);
   }
}

void Translator::translate_file(std::string const& trace_file_name)
{
   spdlog::debug("translate_file");
   std::ifstream trace_file(trace_file_name, std::ios::binary);
   const std::string trim_file_name = trace_file_name + ".trim.log";
   std::ofstream trimmed_file(trim_file_name);
   add_pre_text();

   for (std::string line; getline(trace_file, line);) {
      if (is_deleted(line)) {
         continue;
      }
      replace_words(&line);
      write_to_file(line, trimmed_file);
      translate(line);
   }
   update_count();
   validate_pairs();
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
   auto execute = [](auto& command) {
      std::cout << "Executing...\n" << command << std::endl;
      if (const int returnval = system(command.c_str())) {
         std::cerr << TAG_EXECUTE << " : " << command << " execution failed with code " << returnval << "\n";
         return false;
      }
      return true;
   };

   rgs::all_of(config_.get_execute_commands(), execute);
}

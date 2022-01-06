#pragma once
#include <regex>
#include <string>
#include <vector>
#include "config_types.h"
#include "configparser.h"

namespace unit_test {
class TranslatorTester;
}

class Translator {
   std::string fetch_values_regex(std::string const &line, std::vector<Logalizer::Config::variable> const &variables);
   std::string fetch_values_braced(std::string const &line, std::vector<Logalizer::Config::variable> const &variables);
   std::string capture_values(Logalizer::Config::variable const &var, std::string const &content);
   std::vector<std::string> fetch_values(std::string const &line,
                                         std::vector<Logalizer::Config::variable> const &variables);
   std::string pack_parameters(std::vector<std::string> const &v);
   std::string fill_values_formatted(std::vector<std::string> const &values, std::string const &line_to_fill);
   std::string fill_values(std::vector<std::string> const &values, std::string const &line_to_fill);
   [[nodiscard]] bool is_blacklisted(std::string const &line, std::vector<std::string> const &blacklists);
   auto match(std::string const &line, std::vector<Logalizer::Config::translation> const &translations,
              std::vector<std::string> const &blacklists);
   [[nodiscard]] bool is_deleted(std::string const &line, std::vector<std::string> const &delete_lines,
                                 std::vector<std::regex> const &delete_lines_regex) noexcept;
   void replace(std::string *line, std::vector<Logalizer::Config::replacement> const &replacements);
   void add_translation(std::vector<std::string> &translations, std::string &&translation,
                        const Logalizer::Config::translation trans_cfg,
                        std::unordered_map<size_t, size_t> &trans_count);
   void update_count(std::vector<std::string> &translations, std::unordered_map<size_t, size_t> const &trans_count);

  public:
   void translate_file(std::string const &trace_file_name, std::string const &translation_file_name,
                       Logalizer::Config::ConfigParser const &config);
   friend class ::unit_test::TranslatorTester;
};

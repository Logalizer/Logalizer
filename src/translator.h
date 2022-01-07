#pragma once
#include <regex>
#include <string>
#include <vector>
#include "config_types.h"
#include "configparser.h"

namespace unit_test {
class TranslatorTesterProxy;
}

class Translator {
   std::string fetch_values_regex(std::string const &line, std::vector<Logalizer::Config::variable> const &variables);
   std::string fetch_values_braced(std::string const &line, std::vector<Logalizer::Config::variable> const &variables);
   std::string capture_values(Logalizer::Config::variable const &var, std::string const &content);
   std::vector<std::string> variable_values(std::string const &line,
                                            std::vector<Logalizer::Config::variable> const &variables);
   std::string pack_parameters(std::vector<std::string> const &v);
   std::string fill_values_formatted(std::vector<std::string> const &values, std::string const &line_to_fill);
   std::string update_variables(std::vector<std::string> const &values, std::string const &line_to_fill);
   [[nodiscard]] bool is_blacklisted(std::string const &line);
   auto get_matching_translator(std::string const &line);
   [[nodiscard]] bool is_deleted(std::string const &line) noexcept;
   void replace_words(std::string *line);
   void add_translation(std::string &&translation, Logalizer::Config::duplicates_t duplicates,
                        std::unordered_map<size_t, size_t> &trans_count);
   void update_count(std::unordered_map<size_t, size_t> const &trans_count);
   void add_pre_text();
   void add_post_text();
   void write_translation_file();
   void write_to_file(std::string const &line, std::ofstream &trimmed_file);
   void translate(std::string const &line);
   const Logalizer::Config::ConfigParser &config_;
   std::vector<std::string> translations;

  public:
   Translator(const Logalizer::Config::ConfigParser &config) : config_(config)
   {
   }
   void translate_file(std::string const &trace_file_name);
   friend class ::unit_test::TranslatorTesterProxy;
};

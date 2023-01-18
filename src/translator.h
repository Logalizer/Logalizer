#pragma once
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include "config_types.h"
#include "configparser.h"

namespace unit_test {
class TranslatorTesterProxy;
}

/**
 * @brief Translator is used to translate input file line by line
 *
 *
 */
class Translator {
   std::string fetch_values_regex(std::string const& line, std::vector<Logalizer::Config::variable> const& variables);
   std::string fetch_values_braced(std::string const& line, std::vector<Logalizer::Config::variable> const& variables);
   std::string capture_values(Logalizer::Config::variable const& var, std::string const& content);
   std::vector<std::string> variable_values(std::string const& line,
      std::vector<Logalizer::Config::variable> const& variables);
   std::string pack_parameters(std::vector<std::string> const& v);
   std::string fill_values(std::string const& line, Logalizer::Config::translation const& tr);
   std::string fill_values_formatted(std::vector<std::string> const& values, std::string const& line_to_fill);
   std::string update_variables(std::vector<std::string> const& values, std::string const& line_to_fill);
   [[nodiscard]] bool is_blacklisted(std::string const& line);
   auto get_matching_translator(std::string const& line);
   [[nodiscard]] bool is_deleted(std::string const& line) noexcept;
   void replace_words(std::string* line);
   void add_translation(std::string&& translation, Logalizer::Config::duplicates_t duplicates);
   void update_count();
   void add_pre_text();
   void add_post_text();
   void write_translation_file();
   void write_to_file(std::string const& line, std::ofstream& trimmed_file);
   void translate(std::string const& line);
   const Logalizer::Config::ConfigParser& config_;
   std::vector<std::string> translations;
   std::unordered_map<size_t, size_t> trans_count;

public:
   /**
    * @brief Construct a new Translator object
    *
    * Translation is performed based on configurations provided by ConfigParser
    *
    * @param config
    */
   Translator(const Logalizer::Config::ConfigParser& config) : config_(config)
   {
   }

   /**
    * @brief Translate input file line by line
    *
    * Algorithm
    *
    * 1. Writes contents of wrap_text_pre to translation file
    * 2. Parse intput file line by line
    * 3. Removes lines matching delete_lines in input file
    * 4. Replaces text in input file as configured in replace_words
    * 5. If a line is blacklisted, parse next line
    * 6. If a line matches all the patterns, prepare to write contents of print to translation file
    * 7. Update the print line variables with values as configured in variables
    * 8. Handle duplicates in translation file as per configuration
    * 9. After parsing all lines, update count variables in translation file
    * 10. Write to translation file
    * 11. Writes contents of wrap_text_post
    *
    *  @startuml{myimage.png} "" width=5cm
    *    start
    *    :Write contents of wrap_text_pre to translation file;
    *    :Remove lines matching delete_lines in input file;
    *    :Replaces text in input file as configured in replace_words;
    *    repeat :Read input file line by line;
    *      if (line not is blacklisted? and line matches all the pattern?) equals (Yes) then
    *          :Update variables in print configuration;
    *          :Write that to translation file;
    *      else (No)
    *          :ignore line;
    *       endif
    *    repeat while (more lines?)
    *    :update count variables in translation file;
    *    :Write contents of wrap_text_post to translation file;
    *    stop
    *  @enduml
    *
    * @param trace_file_name
    */
   void translate_file(std::string const& trace_file_name);

   /**
    * @brief Execute configured commands one by one
    *
    */
   void execute_commands();
   friend class ::unit_test::TranslatorTesterProxy;
};

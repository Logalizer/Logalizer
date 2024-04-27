#include "translator.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "configparser_mock.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;
using namespace Logalizer::Config;
using namespace unit_test;

TEST_CASE("is_blacklisted")
{
   ConfigParserMock config;
   std::vector<std::string> blacklist = {"b1", "b2"};
   config.set_blacklists(blacklist);

   TranslatorTesterProxy tr(Translator{config});
   std::string line;

   line = "this is a text";
   CHECK_FALSE(tr.is_blacklisted(line));

   line = "this is a text with b1";
   CHECK(tr.is_blacklisted(line));

   line = "this is a text with b2";
   CHECK(tr.is_blacklisted(line));
}

TEST_CASE("is_deleted")
{
   ConfigParserMock config;
   std::vector<std::string> deleted = {"d1", "d2"};
   std::vector<std::regex> deleted_regex = {std::regex(
       " _r.*x_ ", std::regex_constants::grep | std::regex_constants::nosubs | std::regex_constants::optimize)};
   config.set_delete_lines(deleted);
   config.set_delete_lines_regex(deleted_regex);

   TranslatorTesterProxy tr(Translator{config});
   std::string line;

   line = "this is a text for _regex_ testing";
   CHECK(tr.is_deleted(line));

   line = "this is some text with d1";
   CHECK(tr.is_deleted(line));

   line = "this is some text with d2";
   CHECK(tr.is_deleted(line));

   line = "this is some text";
   CHECK_FALSE(tr.is_deleted(line));
}

TEST_CASE("replace")
{
   ConfigParserMock config;
   std::vector<replacement> replacements = {{"s1", "r1"}, {"s2", "r2"}};
   config.set_replace_words(replacements);

   TranslatorTesterProxy tr(Translator{config});
   std::string line;
   line = "Text with s1 S1 s2 S2";
   std::string expected = "Text with r1 S1 r2 S2";
   tr.replace(&line);

   spdlog::debug("Testing replace");
   CHECK(line == expected);
}

TEST_CASE("translate basic patterns and print with manual variable capture")
{
   std::string tr_file = (fs::temp_directory_path() / "tr.txt").string();
   std::string in_file = (fs::temp_directory_path() / "input.log").string();
   std::ofstream file(in_file);
   ConfigParserMock config;
   config.set_translation_file(tr_file);
   std::vector<translation> translations;
   Translator tor(config);
   std::string read_line;
   std::vector<std::string> lines;

   SECTION("Single pattern and print")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "TemperatureChanged");
   }

   SECTION("Multiple patterns and print")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "TemperatureChanged");
   }

   SECTION("Auto variable capture")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", "C"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(45)");
   }

   SECTION("Auto variable capture with empty end point that captures till the end")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45Celcius";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", ""}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(45Celcius)");
   }

   SECTION("Auto variable capture with endline as end point that captures till the end")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45Celcius";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", "\n"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(45Celcius)");
   }
   SECTION("Auto variable capture with no matching end point that captures till the end")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45Celcius";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", "Farenheit"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(45Celcius)");
   }

   SECTION("Manual variable capture")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature is ${1} degrees";
      tr.variables = {{"= ", "C"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature is 45 degrees");
   }

   SECTION("Auto multi variable capture")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C, state = HighTemp;";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", "C"}, {"state = ", ";"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(45, HighTemp)");
   }

   SECTION("Auto multi variable capture out of order variables")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C, state = HighTemp;";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"state = ", ";"}, {"= ", "C"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(HighTemp, 45)");
   }

   SECTION("Manual multi variable capture out of order variables")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C, state = HighTemp;";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature(${2} - ${1})";
      tr.variables = {{"state = ", ";"}, {"= ", "C"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature(45 - HighTemp)");
   }

   SECTION("Variable capture with missing variables")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C, state = HighTemp;";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"xstate = x", "x;x"}, {"x= x", "xCx"}};
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      getline(read_file, read_line);
      CHECK(read_line == "Temperature( ,  )");
   }
   SECTION("duplicates: default(allowed)")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "TemperatureChanged");
      CHECK(lines.at(1) == "TemperatureChanged");
   }

   SECTION("duplicates: allowed")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged";
      tr.duplicates = duplicates_t::allowed;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "TemperatureChanged");
      CHECK(lines.at(1) == "TemperatureChanged");
   }

   SECTION("duplicates: remove")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged";
      tr.duplicates = duplicates_t::remove;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 1);
      CHECK(lines.at(0) == "TemperatureChanged");
   }

   SECTION("duplicates: remove_continuous")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged";
      tr.duplicates = duplicates_t::remove_continuous;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 1);
      CHECK(lines.at(0) == "TemperatureChanged");
   }

   SECTION("duplicates: remove_continuous not continuous")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", "C"}};
      tr.duplicates = duplicates_t::remove_continuous;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "Temperature(45)");
      CHECK(lines.at(1) == "Temperature(49)");
   }

   SECTION("duplicates: remove_continuous not continuous")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: TemperatureSensor: temperature = 46C\n";
      file << "[INFO]: TemperatureSensor: temperature = 46C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "Temperature";
      tr.variables = {{"= ", "C"}};
      tr.duplicates = duplicates_t::remove_continuous;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 3);
      CHECK(lines.at(0) == "Temperature(45)");
      CHECK(lines.at(1) == "Temperature(49)");
      CHECK(lines.at(2) == "Temperature(46)");
   }

   SECTION("duplicates: count")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged ${count} times";
      tr.duplicates = duplicates_t::count;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 1);
      CHECK(lines.at(0) == "TemperatureChanged 3 times");
   }

   SECTION("duplicates: count_continuous")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor", "temperature"};
      tr.print = "TemperatureChanged to ${1} ${count} times";
      tr.variables = {{"= ", "C"}};
      tr.duplicates = duplicates_t::count;
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "TemperatureChanged to 45 1 times");
      CHECK(lines.at(1) == "TemperatureChanged to 49 2 times");
   }

   SECTION("blacklist")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: PressureSensor: pressure = 14psi\n";
      file.close();
      translation tr;
      config.set_blacklists({"Temperature"});
      tr.patterns = {"Sensor"};
      tr.print = "Sensor values changed";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 1);
      CHECK(lines.at(0) == "Sensor values changed");
   }

   SECTION("Multiple translations")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: PressureSensor: pressure = 14psi\n";
      file.close();
      translation tr;
      tr.patterns = {"TemperatureSensor"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      tr.patterns = {"PressureSensor"};
      tr.print = "PressureChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 3);
      CHECK(lines.at(0) == "TemperatureChanged");
      CHECK(lines.at(1) == "TemperatureChanged");
      CHECK(lines.at(2) == "PressureChanged");
   }

   SECTION("auto_new_line")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: PressureSensor: pressure = 14psi\n";
      file << "[INFO]: HumiditySensor: humidity = 20%\n";
      file.close();
      config.set_auto_new_line_(false);
      translation tr;
      tr.category = "TemperatureSensing";
      tr.patterns = {"TemperatureSensor"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      tr.category = "PressureSensing";
      tr.patterns = {"PressureSensor"};
      tr.print = "PressureChanged";
      translations.push_back(tr);
      tr.patterns = {"HumiditySensor"};
      tr.print = "\nHumidityChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "TemperatureChangedPressureChanged");
      CHECK(lines.at(1) == "HumidityChanged");
   }

   SECTION("wrap_text_pre")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file.close();
      config.set_wrap_text_pre({"pre1", "pre2"});
      translation tr;
      tr.category = "TemperatureSensing";
      tr.patterns = {"TemperatureSensor"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 3);
      CHECK(lines.at(0) == "pre1");
      CHECK(lines.at(1) == "pre2");
      CHECK(lines.at(2) == "TemperatureChanged");
   }

   SECTION("wrap_text_post")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file.close();
      config.set_wrap_text_post({"post1", "post2"});
      translation tr;
      tr.category = "TemperatureSensing";
      tr.patterns = {"TemperatureSensor"};
      tr.print = "TemperatureChanged";
      translations.push_back(tr);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 3);
      CHECK(lines.at(0) == "TemperatureChanged");
      CHECK(lines.at(1) == "post1");
      CHECK(lines.at(2) == "post2");
   }

   SECTION("delete_lines in input file")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: TemperatureSensor: temperature = 49C\n";
      file << "[INFO]: PressureSensor: pressure = 14psi\n";
      file << "[INFO]: HumiditySensor: humidity = 20%\n";
      file.close();
      translation tr;
      tr.patterns = {"Sensor"};
      tr.print = "Sensor values changed";
      translations.push_back(tr);
      config.set_delete_lines({"Temperature"});
      config.set_delete_lines_regex({std::regex("Pres.*: ")});
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      std::ifstream in_file_read(in_file);
      for (std::string read_line; getline(in_file_read, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 1);
      CHECK(lines.at(0) == "[INFO]: HumiditySensor: humidity = 20%");
      lines.clear();
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 1);
      CHECK(lines.at(0) == "Sensor values changed");
   }

   SECTION("replace_words in input file")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C state = 3\n";
      file << "[INFO]: TemperatureSensor: temperature = 59C state = 30\n";
      file.close();
      translation tr;
      tr.patterns = {"Sensor"};
      tr.print = "Temperature";
      tr.variables = {{"state = ", ""}};
      translations.push_back(tr);
      config.set_replace_words({{"state = 30", "state = Very_High_Temp"}, {"state = 3", "state = High_Temp"}});
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      std::ifstream in_file_read(in_file);
      for (std::string read_line; getline(in_file_read, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "[INFO]: TemperatureSensor: temperature = 45C state = High_Temp");
      CHECK(lines.at(1) == "[INFO]: TemperatureSensor: temperature = 59C state = Very_High_Temp");
      lines.clear();
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "Temperature(High_Temp)");
      CHECK(lines.at(1) == "Temperature(Very_High_Temp)");
   }

   SECTION("replace_words in input file with order check")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C state = 1\n";
      file << "[INFO]: TemperatureSensor: temperature = 59C state = 10\n";
      file.close();
      translation tr;
      tr.patterns = {"Sensor"};
      tr.print = "Temperature";
      tr.variables = {{"state = ", ""}};
      translations.push_back(tr);
      config.set_replace_words({{"state = 1", "state = Very_High_Temp"}, {"state = 10", "state = High_Temp"}});
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      std::ifstream in_file_read(in_file);
      for (std::string read_line; getline(in_file_read, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "[INFO]: TemperatureSensor: temperature = 45C state = Very_High_Temp");
      CHECK(lines.at(1) == "[INFO]: TemperatureSensor: temperature = 59C state = Very_High_Temp0");
      lines.clear();
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "Temperature(Very_High_Temp)");
      CHECK(lines.at(1) == "Temperature(Very_High_Temp0)");
   }

   SECTION("replace_words in input file with order check improved config")
   {
      file << "[INFO]: TemperatureSensor: temperature = 45C state = 1\n";
      file << "[INFO]: TemperatureSensor: temperature = 59C state = 10\n";
      file.close();
      translation tr;
      tr.patterns = {"Sensor"};
      tr.print = "Temperature";
      tr.variables = {{"state = ", ""}};
      translations.push_back(tr);
      config.set_replace_words({{"state = 10", "state = Very_High_Temp"}, {"state = 1", "state = High_Temp"}});
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      std::ifstream in_file_read(in_file);
      for (std::string read_line; getline(in_file_read, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "[INFO]: TemperatureSensor: temperature = 45C state = High_Temp");
      CHECK(lines.at(1) == "[INFO]: TemperatureSensor: temperature = 59C state = Very_High_Temp");
      lines.clear();
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "Temperature(High_Temp)");
      CHECK(lines.at(1) == "Temperature(Very_High_Temp)");
   }

   SECTION("check if a pair is matching")
   {
      file << "Alice said Hi to Bob\n";
      file << "some line\n";
      file << "some line\n";
      file << "Bob replied Hi\n";
      file.close();

      translation tr_a2b;
      tr_a2b.patterns = {"Alice", "Bob"};
      tr_a2b.print = "A -> B: ${1}";
      tr_a2b.variables = {{"said ", " to"}};
      pair pr = {"A -> B", "B -> A", "A -> B", "error print"};
      std::vector<pair> pairs;
      pairs.push_back(pr);

      translation tr_b2a;
      tr_b2a.patterns = {"Bob replied"};
      tr_b2a.print = "B -> A: Responded";

      translation some;
      some.patterns = {"some"};
      some.print = "some";

      translations.push_back(tr_a2b);
      translations.push_back(tr_b2a);
      translations.push_back(some);

      config.set_pairs(pairs);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 4);
      CHECK(lines.at(0) == "A -> B: Hi");
      CHECK(lines.at(1) == "some");
      CHECK(lines.at(2) == "some");
      CHECK(lines.at(3) == "B -> A: Responded");
   }

   SECTION("check if a pair is matching in subsequent lines")
   {
      file << "Alice said Hi to Bob\n";
      file << "some line\n";
      file << "some line\n";
      file << "some line\n";
      file << "Bob replied Hi\n";
      file.close();

      translation tr_a2b;
      tr_a2b.patterns = {"Alice", "Bob"};
      tr_a2b.print = "A -> B: ${1}";
      tr_a2b.variables = {{"said ", " to"}};

      pair pr = {"A -> B", "B -> A", "A -> B", "error print"};
      std::vector<pair> pairs;
      pairs.push_back(pr);

      translation tr_b2a;
      tr_b2a.patterns = {"Bob replied"};
      tr_b2a.print = "B -> A: Responded";

      translations.push_back(tr_a2b);
      translations.push_back(tr_b2a);
      config.set_pairs(pairs);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "A -> B: Hi");
      CHECK(lines.at(1) == "B -> A: Responded");
   }

   SECTION("check if a pair is not matching")
   {
      file << "Alice said Hi to Bob\n";
      file << "some line\n";
      file << "some line\n";
      file << "some line\n";
      file << "Alice said Hi to Bob\n";
      file << "Bob replied Hi\n";
      file.close();

      translation tr_a2b;
      tr_a2b.patterns = {"Alice", "Bob"};
      tr_a2b.print = "A -> B: ${1}";
      tr_a2b.variables = {{"said ", " to"}};

      pair pr = {"A -> B", "B -> A", "A -> B", "error print"};
      std::vector<pair> pairs;
      pairs.push_back(pr);

      translation tr_b2a;
      tr_b2a.patterns = {"Bob replied"};
      tr_b2a.print = "B -> A: Responded";

      translations.push_back(tr_a2b);
      translations.push_back(tr_b2a);
      config.set_pairs(pairs);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 4);
      CHECK(lines.at(0) == "A -> B: Hi");
      CHECK(lines.at(1) == "error print");
      CHECK(lines.at(2) == "A -> B: Hi");
      CHECK(lines.at(3) == "B -> A: Responded");
   }

   SECTION("check if a pair is not matching multiple start with one end")
   {
      file << "Alice said Hi to Bob\n";
      file << "Alice said Hi to Bob\n";
      file << "some line\n";
      file << "some line\n";
      file << "some line\n";
      file << "Bob replied Hi\n";
      file.close();

      translation tr_a2b;
      tr_a2b.patterns = {"Alice", "Bob"};
      tr_a2b.print = "A -> B: ${1}";
      tr_a2b.variables = {{"said ", " to"}};

      pair pr = {"A -> B", "B -> A", "A -> B", "error print"};
      std::vector<pair> pairs;
      pairs.push_back(pr);

      translation tr_b2a;
      tr_b2a.patterns = {"Bob replied"};
      tr_b2a.print = "B -> A: Responded";

      translations.push_back(tr_a2b);
      translations.push_back(tr_b2a);
      config.set_pairs(pairs);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 4);
      CHECK(lines.at(0) == "A -> B: Hi");
      CHECK(lines.at(1) == "error print");
      CHECK(lines.at(2) == "A -> B: Hi");
      CHECK(lines.at(3) == "B -> A: Responded");
   }

   SECTION("check if a pair is not matching and terminator not matching")
   {
      file << "Alice said Hi to Bob\n";
      file << "some line\n";
      file << "some line\n";
      file << "some line\n";
      file.close();

      translation tr_a2b;
      tr_a2b.patterns = {"Alice", "Bob"};
      tr_a2b.print = "A -> B: ${1}";
      tr_a2b.variables = {{"said ", " to"}};

      pair pr = {"A -> B", "B -> A", "A -> B", "error print"};
      std::vector<pair> pairs;
      pairs.push_back(pr);

      translation tr_b2a;
      tr_b2a.patterns = {"Bob replied"};
      tr_b2a.print = "B -> A: Responded";

      translations.push_back(tr_a2b);
      translations.push_back(tr_b2a);
      config.set_pairs(pairs);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 2);
      CHECK(lines.at(0) == "A -> B: Hi");
      CHECK(lines.at(1) == "error print");
   }

   SECTION("check if a pair is not matching multiple start with no end")
   {
      file << "Alice said Hi to Bob\n";
      file << "Alice said Hi to Bob\n";
      file << "some line\n";
      file << "some line\n";
      file << "some line\n";
      file.close();

      translation tr_a2b;
      tr_a2b.patterns = {"Alice", "Bob"};
      tr_a2b.print = "A -> B: ${1}";
      tr_a2b.variables = {{"said ", " to"}};

      pair pr = {"A -> B", "B -> A", "A -> B", "error print"};
      std::vector<pair> pairs;
      pairs.push_back(pr);

      translation tr_b2a;
      tr_b2a.patterns = {"Bob replied"};
      tr_b2a.print = "B -> A: Responded";

      translations.push_back(tr_a2b);
      translations.push_back(tr_b2a);
      config.set_pairs(pairs);
      config.set_translations(translations);
      tor.translate_file(in_file);
      std::ifstream read_file(tr_file);
      for (std::string read_line; getline(read_file, read_line);) {
         lines.push_back(read_line);
      }
      CHECK(lines.size() == 4);
      CHECK(lines.at(0) == "A -> B: Hi");
      CHECK(lines.at(1) == "error print");
      CHECK(lines.at(2) == "A -> B: Hi");
      CHECK(lines.at(3) == "error print");
   }
}

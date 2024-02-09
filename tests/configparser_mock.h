#pragma once
#include <utility>

#include "configparser.h"
#include "translator.h"
namespace unit_test {

using namespace Logalizer::Config;
class ConfigParserMock : public Logalizer::Config::ConfigParser {
  public:
   void read_config_file() override
   {
   }
   void load_disabled_categories() override
   {
   }
   void load_translations() override
   {
   }
   void load_wrap_text() override
   {
   }
   void load_pairs() override
   {
   }
   void load_blacklists() override
   {
   }
   void load_delete_lines() override
   {
   }
   void load_replace_words() override
   {
   }
   void load_execute() override
   {
   }
   void load_translation_file() override
   {
   }
   void load_backup_file() override
   {
   }
   void load_auto_new_line() override
   {
   }
   ConfigParserMock() = default;
   void set_translations(std::vector<translation> translations)
   {
      ConfigParser::set_translations(std::move(translations));
   }

   void set_pairs(std::vector<pair> pairs)
   {
      ConfigParser::set_pairs(std::move(pairs));
   }

   void set_disabled_categories(std::vector<std::string> disabled_categories)
   {
      ConfigParser::set_disabled_categories(std::move(disabled_categories));
   }

   void set_wrap_text_pre(std::vector<std::string> wrap_text_pre)
   {
      ConfigParser::set_wrap_text_pre(std::move(wrap_text_pre));
   }

   void set_wrap_text_post(std::vector<std::string> wrap_text_post)
   {
      ConfigParser::set_wrap_text_post(std::move(wrap_text_post));
   }

   void set_delete_lines_regex(std::vector<std::regex> delete_lines_regex)
   {
      ConfigParser::set_delete_lines_regex(std::move(delete_lines_regex));
   }

   void set_delete_lines(std::vector<std::string> delete_lines)
   {
      ConfigParser::set_delete_lines(std::move(delete_lines));
   }

   void set_replace_words(std::vector<replacement> replace_words)
   {
      ConfigParser::set_replace_words(std::move(replace_words));
   }

   void set_blacklists(std::vector<std::string> blacklists)
   {
      ConfigParser::set_blacklists(std::move(blacklists));
   }

   void set_execute_commands(std::vector<std::string> execute_commands)
   {
      ConfigParser::set_execute_commands(std::move(execute_commands));
   }

   void set_translation_file(std::string translation_file)
   {
      ConfigParser::set_translation_file(std::move(translation_file));
   }

   void set_backup_file(std::string backup_file)
   {
      ConfigParser::set_backup_file(std::move(backup_file));
   }

   void set_auto_new_line_(bool auto_new_line)
   {
      ConfigParser::set_auto_new_line(auto_new_line);
   }
};

class TranslatorTesterProxy {
  public:
   explicit TranslatorTesterProxy(Translator translator) : tr(std::move(translator))
   {
   }
   [[nodiscard]] bool is_blacklisted(std::string const &line)
   {
      return tr.is_blacklisted(line);
   }
   [[nodiscard]] bool is_deleted(std::string const &line) noexcept
   {
      return tr.is_deleted(line);
   }
   void replace(std::string *line)
   {
      tr.replace_words(line);
   }

  private:
   Translator tr;
};

}  // namespace unit_test

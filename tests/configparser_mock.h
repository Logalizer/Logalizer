#pragma once
#include "configparser.h"
#include "translator.h"
namespace unit_test {

class ConfigParserMock : public Logalizer::Config::ConfigParser {
  public:
   void read_config_file()
   {
   }
   void load_disabled_categories()
   {
   }
   void load_translations()
   {
   }
   void load_wrap_text()
   {
   }
   void load_blacklists()
   {
   }
   void load_delete_lines()
   {
   }
   void load_replace_words()
   {
   }
   void load_execute()
   {
   }
   void load_translation_file()
   {
   }
   void load_backup_file()
   {
   }
   void load_auto_new_line()
   {
   }
   ConfigParserMock() : ConfigParser("")
   {
   }
};

class TranslatorTesterProxy {
  public:
   TranslatorTesterProxy(Translator translator) : tr(translator)
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

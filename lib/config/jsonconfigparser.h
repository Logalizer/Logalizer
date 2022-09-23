#pragma once

#include "configparser.h"
#include "external/json/single_include/nlohmann/json.hpp"

namespace Logalizer::Config {

class JsonConfigParser final : public ConfigParser {
  public:
   JsonConfigParser(const std::string &config_file = "config.json");
   JsonConfigParser(nlohmann::json config);
   virtual void load_disabled_categories() override;
   virtual void load_translations() override;
   virtual void load_wrap_text() override;
   virtual void load_blacklists() override;
   virtual void load_delete_lines() override;
   virtual void load_replace_words() override;
   virtual void load_execute() override;
   virtual void load_translation_file() override;
   virtual void load_backup_file() override;
   virtual void load_auto_new_line() override;

   virtual void read_config_file() override;

  private:
   nlohmann::json config_;
   std::string config_file_;
   template <class T>
   T get_value_or(nlohmann::json const &config, std::string const &name, T value);
   std::vector<variable> get_variables(nlohmann::json const &config);
   std::vector<translation> load_translations(nlohmann::json const &config, std::string const &name,
                                              std::vector<std::string> const &disabled_categories);
};
}  // namespace Logalizer::Config

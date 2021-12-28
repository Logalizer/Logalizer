#pragma once

#include "configparser.h"
#include "external/json/single_include/nlohmann/json.hpp"

namespace Logalizer::Config {

class JsonConfigParser final : public ConfigParser {
  public:
   JsonConfigParser(const std::string &config_file = "config.json");
   virtual void load_translations() override;
   virtual void load_wrap_text() override;
   virtual void load_blacklists() override;
   virtual void load_delete_lines() override;
   virtual void load_replace_words() override;
   virtual void load_execute() override;
   virtual void load_translation_file() override;
   virtual void load_backup_file() override;
   virtual void load_auto_new_line() override;

   virtual void load_config_file() override;
   virtual void load_all_configurations() override;

  private:
   nlohmann::json config_;
};
}  // namespace Logalizer::Config

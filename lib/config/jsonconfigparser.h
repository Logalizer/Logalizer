#pragma once

#include "configparser.h"
#include "external/json/single_include/nlohmann/json.hpp"

namespace Logalizer::Config {

class JsonConfigParser : public ConfigParser {
  public:
   JsonConfigParser(const std::string &config_file = "config.json");
   virtual void loadTranslations() override;
   virtual void loadWrapText() override;
   virtual void loadBlacklists() override;
   virtual void loadDeleteLines() override;
   virtual void loadReplaceWords() override;
   virtual void loadGenerateUML() override;
   virtual void loadUMLFile() override;
   virtual void loadBackupFile() override;
   virtual ~JsonConfigParser() override;

   virtual void loadConfigFile() override;
   virtual void loadAllConfigurations() override;

  private:
   nlohmann::json config_;
};
}  // namespace Logalizer::Config

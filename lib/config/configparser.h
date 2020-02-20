#pragma once

#include <regex>
#include "config_types.h"

namespace Logalizer::Config {

constexpr const char TAG_TRANSLATIONS[] = "translations";
constexpr char TAG_WRAPTEXT_PRE[] = "wrap_text_pre";
constexpr char TAG_WRAPTEXT_POST[] = "wrap_text_post";
constexpr char TAG_DISABLE_CATEGORY[] = "disable_category";
constexpr char TAG_BLACKLIST[] = "blacklist";
constexpr char TAG_DELETE_LINES[] = "delete_lines";
constexpr char TAG_REPLACE_WORDS[] = "replace_words";
constexpr char TAG_EXECUTE[] = "execute";
constexpr char TAG_TRANSLATION_FILE[] = "translation_file";
constexpr char TAG_BACKUP_FILE[] = "backup_file";

class ConfigParser {
  public:
   ConfigParser(std::string config_file);
   virtual void loadConfigFile() = 0;
   virtual void loadAllConfigurations() = 0;
   virtual void loadTranslations() = 0;
   virtual void loadWrapText() = 0;
   virtual void loadBlacklists() = 0;
   virtual void loadDeleteLines() = 0;
   virtual void loadReplaceWords() = 0;
   virtual void loadExecute() = 0;
   virtual void loadTranslationFile() = 0;
   virtual void loadBackupFile() = 0;
   virtual ~ConfigParser() = default;
   void updateRelativePaths(std::string const& log_file);

  protected:
   std::vector<translation> translations_;
   std::vector<std::string> disabled_categories_;
   std::vector<std::string> wrap_text_pre_;
   std::vector<std::string> wrap_text_post_;
   std::vector<std::regex> delete_lines_regex_;
   std::vector<std::string> delete_lines_;
   std::vector<replacement> replace_words_;
   std::vector<std::string> blacklists_;
   std::vector<std::string> execute_commands_;
   std::string translation_file_;
   std::string backup_file_;

   std::string config_file_;

  public:
   [[nodiscard]] inline std::vector<translation> const& getTranslations() const noexcept
   {
      return translations_;
   }

   [[nodiscard]] inline std::vector<std::string> const& getDisabledCategories() const noexcept
   {
      return disabled_categories_;
   }

   [[nodiscard]] inline std::vector<std::string> const& getWrapTextPre() const noexcept
   {
      return wrap_text_pre_;
   }

   [[nodiscard]] inline std::vector<std::string> const& getWrapTextPost() const noexcept
   {
      return wrap_text_post_;
   }

   [[nodiscard]] inline std::vector<std::regex> const& getDeleteLinesRegex() const noexcept
   {
      return delete_lines_regex_;
   }
   [[nodiscard]] inline std::vector<std::string> const& getDeleteLines() const noexcept
   {
      return delete_lines_;
   }
   [[nodiscard]] inline std::vector<replacement> const& getReplaceWords() const noexcept
   {
      return replace_words_;
   }
   [[nodiscard]] inline std::vector<std::string> const& getBlacklists() const noexcept
   {
      return blacklists_;
   }
   [[nodiscard]] inline std::vector<std::string> const& getExecuteCommands() const noexcept
   {
      return execute_commands_;
   }
   [[nodiscard]] inline std::string const& getTranslationFile() const noexcept
   {
      return translation_file_;
   }

   [[nodiscard]] inline std::string const& getBackupFile() const noexcept
   {
      return backup_file_;
   }
};
}  // namespace Logalizer::Config

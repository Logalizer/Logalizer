#pragma once

#include <regex>
#include "config_types.h"

namespace Logalizer::Config {

constexpr char TAG_TRANSLATIONS[] = "translations";
constexpr char TAG_WRAPTEXT_PRE[] = "wrap_text_pre";
constexpr char TAG_WRAPTEXT_POST[] = "wrap_text_post";
constexpr char TAG_DISABLE_CATEGORY[] = "disable_category";
constexpr char TAG_BLACKLIST[] = "blacklist";
constexpr char TAG_DELETE_LINES[] = "delete_lines";
constexpr char TAG_REPLACE_WORDS[] = "replace_words";
constexpr char TAG_EXECUTE[] = "execute";
constexpr char TAG_TRANSLATION_FILE[] = "translation_file";
constexpr char TAG_BACKUP_FILE[] = "backup_file";
constexpr char TAG_AUTO_NEW_LINE[] = "auto_new_line";
constexpr char TAG_CATEGORY[] = "category";
constexpr char TAG_PATTERNS[] = "patterns";
constexpr char TAG_PRINT[] = "print";
constexpr char TAG_VARIABLES[] = "variables";
constexpr char TAG_REPEAT[] = "repeat";
constexpr char TAG_COUNT[] = "count";
constexpr char TAG_COUNT_GLOBAL[] = "global";
constexpr char TAG_COUNT_SCOPED[] = "scoped";

class ConfigParser {
  public:
   ConfigParser(std::string config_file);
   virtual ~ConfigParser() = default;
   ConfigParser(ConfigParser&&) = default;
   ConfigParser(const ConfigParser&) = default;
   ConfigParser& operator=(const ConfigParser&) = default;
   ConfigParser& operator=(ConfigParser&&) = default;

   virtual void read_config_file() = 0;
   virtual void load_translations() = 0;
   virtual void load_wrap_text() = 0;
   virtual void load_blacklists() = 0;
   virtual void load_delete_lines() = 0;
   virtual void load_replace_words() = 0;
   virtual void load_execute() = 0;
   virtual void load_translation_file() = 0;
   virtual void load_backup_file() = 0;
   virtual void load_auto_new_line() = 0;
   void load_configurations();
   void update_relative_paths(std::string const& log_file);

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
   bool auto_new_line_;

   std::string config_file_;

  public:
   [[nodiscard]] inline std::vector<translation> const& get_translations() const noexcept
   {
      return translations_;
   }

   [[nodiscard]] inline std::vector<std::string> const& get_disabled_categories() const noexcept
   {
      return disabled_categories_;
   }

   [[nodiscard]] inline std::vector<std::string> const& get_wrap_text_pre() const noexcept
   {
      return wrap_text_pre_;
   }

   [[nodiscard]] inline std::vector<std::string> const& get_wrap_text_post() const noexcept
   {
      return wrap_text_post_;
   }

   [[nodiscard]] inline std::vector<std::regex> const& get_delete_lines_regex() const noexcept
   {
      return delete_lines_regex_;
   }
   [[nodiscard]] inline std::vector<std::string> const& get_delete_lines() const noexcept
   {
      return delete_lines_;
   }
   [[nodiscard]] inline std::vector<replacement> const& get_replace_words() const noexcept
   {
      return replace_words_;
   }
   [[nodiscard]] inline std::vector<std::string> const& get_blacklists() const noexcept
   {
      return blacklists_;
   }
   [[nodiscard]] inline std::vector<std::string> const& get_execute_commands() const noexcept
   {
      return execute_commands_;
   }
   [[nodiscard]] inline std::string const& get_translation_file() const noexcept
   {
      return translation_file_;
   }

   [[nodiscard]] inline std::string const& get_backup_file() const noexcept
   {
      return backup_file_;
   }

   [[nodiscard]] inline bool const& get_auto_new_line() const noexcept
   {
      return auto_new_line_;
   }
};
}  // namespace Logalizer::Config

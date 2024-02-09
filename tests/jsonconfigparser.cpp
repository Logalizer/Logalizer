#include "jsonconfigparser.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include "config_types.h"

using namespace Logalizer::Config;
using nlohmann::json;

TEST_CASE("wrap_text_pre available")
{
   auto j = R"(
  {
    "wrap_text_pre": [
      "pre1",
      "pre2"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_wrap_text();
   CHECK(parser.get_wrap_text_pre() == std::vector<std::string>({"pre1", "pre2"}));
}

TEST_CASE("wrap_text_pre unavailable")
{
   auto j = R"( { })"_json;

   JsonConfigParser parser(j);
   parser.load_wrap_text();
   CHECK(parser.get_wrap_text_pre() == std::vector<std::string>({}));
}

TEST_CASE("wrap_text_post")
{
   auto j = R"(
  {
    "wrap_text_post": [
      "post1",
      "post2"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_wrap_text();
   CHECK(parser.get_wrap_text_post() == std::vector<std::string>({"post1", "post2"}));
}

TEST_CASE("wrap_text_post unavailable")
{
   auto j = R"( { })"_json;

   JsonConfigParser parser(j);
   parser.load_wrap_text();
   CHECK(parser.get_wrap_text_post() == std::vector<std::string>({}));
}

TEST_CASE("blacklists available")
{
   auto j = R"(
  {
    "blacklist": [
      "bl1",
      "bl2"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_blacklists();
   CHECK(parser.get_blacklists() == std::vector<std::string>({"bl1", "bl2"}));
}

TEST_CASE("blacklists unavailable")
{
   auto j = R"( { })"_json;

   JsonConfigParser parser(j);
   parser.load_blacklists();
   CHECK(parser.get_blacklists() == std::vector<std::string>({}));
}

TEST_CASE("delete_lines available")
{
   auto j = R"(
  {
    "delete_lines": [
      "dl1",
      "dl2",
      "dl_regex.*"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_delete_lines();
   CHECK(parser.get_delete_lines() == std::vector<std::string>({"dl1", "dl2"}));
   CHECK(parser.get_delete_lines_regex().size() == 1);
}

TEST_CASE("delete_lines unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_delete_lines());
   CHECK(parser.get_delete_lines() == std::vector<std::string>({}));
   CHECK(parser.get_delete_lines_regex().empty());
}

TEST_CASE("replace_words available")
{
   auto j = R"(
  {
    "replace_words": {
      "old1": "new1",
      "old2": "new2"
    }
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_replace_words();
   auto replacements = parser.get_replace_words();
   CHECK(replacements.size() == 2);
   CHECK(replacements.at(0).search == "old1");
   CHECK(replacements.at(0).replace == "new1");
   CHECK(replacements.at(1).search == "old2");
   CHECK(replacements.at(1).replace == "new2");
}

TEST_CASE("replace_words unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_replace_words());
   CHECK(parser.get_replace_words().empty());
}

TEST_CASE("execute available")
{
   auto j = R"(
  {
    "execute": [
      "cmd1",
      "cmd2"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_execute();
   CHECK(parser.get_execute_commands() == std::vector<std::string>({"cmd1", "cmd2"}));
}

TEST_CASE("execute unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_execute());
   CHECK(parser.get_execute_commands() == std::vector<std::string>({}));
}

TEST_CASE("disable_group available")
{
   auto j = R"(
  {
    "disable_group": [
      "d1",
      "d2"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_disabled_categories();
   CHECK(parser.get_disabled_categories() == std::vector<std::string>({"d1", "d2"}));
}

TEST_CASE("disable_group unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_disabled_categories());
   CHECK(parser.get_disabled_categories() == std::vector<std::string>({}));
}

TEST_CASE("translation_file available")
{
   auto j = R"(
  {
    "translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt"
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translation_file();
   CHECK(parser.get_translation_file() == "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt");
}

TEST_CASE("translation_file unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_translation_file());
   CHECK(parser.get_translation_file().empty());
}

TEST_CASE("backup_file available")
{
   auto j = R"(
  {
    "backup_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_backup.txt"
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_backup_file();
   CHECK(parser.get_backup_file() == "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_backup.txt");
}

TEST_CASE("backup_file unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_backup_file());
   CHECK(parser.get_backup_file().empty());
}

TEST_CASE("auto_new_line available")
{
   auto j = R"(
  {
    "auto_new_line": false
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_auto_new_line();
   CHECK(parser.get_auto_new_line() == false);
   j = R"(
  {
    "auto_new_line": true
  }
  )"_json;

   JsonConfigParser parsertrue(j);
   parsertrue.load_auto_new_line();
   CHECK(parsertrue.get_auto_new_line() == true);
}

TEST_CASE("auto_new_line unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_auto_new_line());
   CHECK(parser.get_auto_new_line() == true);
}

TEST_CASE("translations_csv available")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,pattern1,pattern2,,v1startswith,v1endswith,v2startswith,v2endswith,,\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.duplicates == duplicates_t::allowed);
}

TEST_CASE("translations_csv available with double quote within double quote")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,\"\"\"pattern1\"\"\",pattern2,,v1startswith,v1endswith,v2startswith,v2endswith,,\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"\"pattern1\"", "pattern2"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.duplicates == duplicates_t::allowed);
}

TEST_CASE("translations_csv available with comma within double quote")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,\"pattern1, pattern1.1\",pattern2,,v1startswith,v1endswith,v2startswith,v2endswith,,\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1, pattern1.1", "pattern2"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.duplicates == duplicates_t::allowed);
}

TEST_CASE("translations_csv available with all fields")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,pattern1,pattern2,pattern3,v1startswith,v1endswith,v2startswith,v2endswith,v3startswith,"
          "v3endswith\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2", "pattern3"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(variables.at(2).startswith == "v3startswith");
   CHECK(variables.at(2).endswith == "v3endswith");
   CHECK(tr.duplicates == duplicates_t::allowed);
}

TEST_CASE("translations_csv without patterns")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,,,,v1startswith,v1endswith,v2startswith,v2endswith,v3startswith,"
          "v3endswith\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.empty());
}

TEST_CASE("translations_csv without variables")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,pattern1,,,,,,,,\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(tr.variables.empty());
   CHECK(tr.duplicates == duplicates_t::allowed);
}

TEST_CASE("translations_csv disabled entry")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv"
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "No,group_name,print this "
          "message,allowed,pattern1,pattern2,pattern3,v1startswith,v1endswith,v2startswith,v2endswith,v3startswith,"
          "v3endswith\n";
   csv << "No,,,,,,,,,,,,\n";
   csv << "no,,,,,,,,,,,,\n";
   csv << "False,,,,,,,,,,,,\n";
   csv << "false,,,,,,,,,,,,\n";
   csv << "0,,,,,,,,,,,,\n";

   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.empty());
}

TEST_CASE("translations_csv both translations and translations_csv")
{
   auto j = R"(
  {
    "translations_csv": "config_translations.csv",
    "translations": [
     {
       "group": "group_name",
       "patterns": [
         "pattern1",
         "pattern2"
       ],
       "print": "print this message",
       "variables": [
         {
           "startswith": "v1startswith",
           "endswith": "v1endswith"
         },
         {
           "startswith": "v2startswith",
           "endswith": "v2endswith"
         }
       ],
       "duplicates": "allowed"
     }
    ]
  }
  )"_json;

   std::ofstream csv("config_translations.csv");
   csv << "enable,group,print,duplicates,pattern1,pattern2,pattern3,variable1_starts_with,variable1_ends_with,"
          "variable2_starts_with,variable2_ends_with,variable3_starts_with,variable3_ends_with\n";
   csv << "Yes,group_name,print this "
          "message,allowed,pattern1,pattern2,pattern3,v1startswith,v1endswith,v2startswith,v2endswith,v3startswith,"
          "v3endswith\n";
   csv.close();

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
}

TEST_CASE("translations one translation available")
{
   auto j = R"( {
    "translations": [
     {
       "group": "group_name",
       "patterns": [
         "pattern1",
         "pattern2"
       ],
       "print": "print this message",
       "variables": [
         {
           "startswith": "v1startswith",
           "endswith": "v1endswith"
         },
         {
           "startswith": "v2startswith",
           "endswith": "v2endswith"
         }
       ],
       "duplicates": "allowed"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.duplicates == duplicates_t::allowed);
}

TEST_CASE("translations group not mandatory")
{
   auto j = R"( {
    "translations": [
     {
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category.empty());
}

TEST_CASE("translations patterns is mandatory")
{
   auto j = R"( {
    "translations": [
     {
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.empty());
}

TEST_CASE("translations print is mandatory")
{
   auto j = R"( {
    "translations": [
     {
       "patterns": [
         "pattern1"
       ]
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.empty());
}

TEST_CASE("translations indivudually disabled")
{
   auto j = R"( {
   "disable_group" : [],
    "translations": [
     {
       "group": "c1",
       "enable": false,
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "group": "c1",
       "enable": true,
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "group": "c2",
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_disabled_categories();
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 2);
   CHECK(trs[0].category == "c1");
   CHECK(trs[1].category == "c2");
}

TEST_CASE("translations group disabled")
{
   auto j = R"( {
   "disable_group" : ["c1"],
    "translations": [
     {
       "group": "c1",
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "group": "c1",
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "group": "c2",
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_disabled_categories();
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "c2");
}

TEST_CASE("translations duplicates types")
{
   auto j = R"( {
    "translations": [
     {
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "duplicates": "remove",
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "duplicates": "remove_continuous",
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "duplicates": "count",
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "duplicates": "count_continuous",
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "duplicates": "allowed",
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 6);
   CHECK(trs.at(0).duplicates == duplicates_t::allowed);
   CHECK(trs.at(1).duplicates == duplicates_t::remove);
   CHECK(trs.at(2).duplicates == duplicates_t::remove_continuous);
   CHECK(trs.at(3).duplicates == duplicates_t::count);
   CHECK(trs.at(4).duplicates == duplicates_t::count_continuous);
   CHECK(trs.at(5).duplicates == duplicates_t::allowed);
}

TEST_CASE("read full configuration")
{
   auto j = R"( {
    "translations": [
     {
       "group": "group_name",
       "patterns": [
         "pattern1",
         "pattern2"
       ],
       "print": "print this message",
       "variables": [
         {
           "startswith": "v1startswith",
           "endswith": "v1endswith"
         },
         {
           "startswith": "v2startswith",
           "endswith": "v2endswith"
         }
       ],
       "duplicates": "remove"
     }
    ],
    "wrap_text_pre": [
      "pre1",
      "pre2"
    ],
    "wrap_text_post": [
      "post1",
      "post2"
    ],
    "blacklist": [
      "bl1",
      "bl2"
    ],
    "delete_lines": [
      "dl1",
      "dl2",
      "dl_regex.*"
    ],
    "replace_words": {
      "old1": "new1",
      "old2": "new2"
    },
    "execute": [
      "cmd1",
      "cmd2"
    ],
    "translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt",
    "backup_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_backup.txt"
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_configurations();
   CHECK(parser.get_auto_new_line() == true);
   CHECK(parser.get_wrap_text_pre().size() == 2);
   CHECK(parser.get_wrap_text_post().size() == 2);
   CHECK(parser.get_blacklists().size() == 2);
   CHECK(parser.get_delete_lines().size() == 2);
   CHECK(parser.get_delete_lines_regex().size() == 1);
   CHECK(parser.get_replace_words().size() == 2);
   CHECK(parser.get_execute_commands().size() == 2);
   CHECK_FALSE(parser.get_translation_file().empty());
   CHECK_FALSE(parser.get_backup_file().empty());

   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.duplicates == duplicates_t::remove);
}

TEST_CASE("read minimal mandatory configuration")
{
   auto j = R"( {
    "translations": [
     {
       "group": "group_name",
       "patterns": [
         "pattern1",
         "pattern2"
       ],
       "print": "print this message",
       "variables": [
         {
           "startswith": "v1startswith",
           "endswith": "v1endswith"
         },
         {
           "startswith": "v2startswith",
           "endswith": "v2endswith"
         }
       ],
       "duplicates": "remove"
     }
    ],
    "translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt"
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_configurations();
   CHECK(parser.get_auto_new_line() == true);
   CHECK(parser.get_wrap_text_pre().empty());
   CHECK(parser.get_wrap_text_post().empty());
   CHECK(parser.get_blacklists().empty());
   CHECK(parser.get_delete_lines().empty());
   CHECK(parser.get_delete_lines_regex().empty());
   CHECK(parser.get_replace_words().empty());
   CHECK(parser.get_execute_commands().empty());
   CHECK_FALSE(parser.get_translation_file().empty());
   CHECK(parser.get_backup_file().empty());

   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "group_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.duplicates == duplicates_t::remove);
}

TEST_CASE("translations unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_translations());
}

TEST_CASE("translations one translation and with pairswith")
{
   auto j = R"( {
    "translations": [
     {
       "group": "group_name",
       "patterns": [
         "pattern1",
         "pattern2"
       ],
       "print": "print this message",
       "variables": [
         {
           "startswith": "v1startswith",
           "endswith": "v1endswith"
         },
         {
           "startswith": "v2startswith",
           "endswith": "v2endswith"
         }
       ],
       "duplicates": "allowed"
     }
    ],
    "pairs": [
      {
        "source": "printmatch",
        "pairswith": "pairmatch",
        "before": "before",
        "error": "error print"
      }
    ],
    "translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt"
  }
  )"_json;
   JsonConfigParser parser(j);
   parser.load_configurations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   const auto& pairs = parser.get_pairs();
   CHECK(pairs.size() == 1);
   CHECK(pairs.at(0).source == "printmatch");
   CHECK(pairs.at(0).pairswith == "pairmatch");
   CHECK(pairs.at(0).before == "before");
   CHECK(pairs.at(0).error == "error print");
}

TEST_CASE("translations one translation and with many pairswith")
{
   auto j = R"( {
    "translations": [
     {
       "group": "group_name",
       "patterns": [
         "pattern1",
         "pattern2"
       ],
       "print": "print this message",
       "variables": [
         {
           "startswith": "v1startswith",
           "endswith": "v1endswith"
         },
         {
           "startswith": "v2startswith",
           "endswith": "v2endswith"
         }
       ],
       "duplicates": "allowed"
     }
    ],
    "pairs": [
      {
        "source": "printmatch",
        "pairswith": "pairmatch",
        "before": "before",
        "error": "error print"
      },
      {
        "source": "printmatch",
        "pairswith": "pairmatch",
        "before": "before",
        "error": "error print"
      }
    ],
    "translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt"
  }
  )"_json;
   JsonConfigParser parser(j);
   parser.load_configurations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   const auto& pairs = parser.get_pairs();
   CHECK(pairs.size() == 2);
   CHECK(pairs.at(0).source == "printmatch");
   CHECK(pairs.at(0).pairswith == "pairmatch");
   CHECK(pairs.at(0).before == "before");
   CHECK(pairs.at(0).error == "error print");

   CHECK(pairs.at(1).source == "printmatch");
   CHECK(pairs.at(1).pairswith == "pairmatch");
   CHECK(pairs.at(1).before == "before");
   CHECK(pairs.at(1).error == "error print");
}

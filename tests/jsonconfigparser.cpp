#include "jsonconfigparser.h"
#include "catch.hpp"
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

TEST_CASE("disable_category available")
{
   auto j = R"(
  {
    "disable_category": [
      "d1",
      "d2"
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_disabled_categories();
   CHECK(parser.get_disabled_categories() == std::vector<std::string>({"d1", "d2"}));
}

TEST_CASE("disable_category unavailable")
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
   CHECK(parser.get_translation_file() == "");
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
   CHECK(parser.get_backup_file() == "");
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

TEST_CASE("translations one translation available")
{
   auto j = R"( {
    "translations": [
     {
       "category": "category_name",
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
       "count": "global"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 1);
   const auto& tr = trs.front();
   CHECK(tr.category == "category_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   CHECK(tr.repeat == true);
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.count == count_type::global);
}

TEST_CASE("translations category not mandatory")
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
   CHECK(tr.category == "");
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
   CHECK(trs.size() == 0);
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
   CHECK(trs.size() == 0);
}

TEST_CASE("translations category disabled")
{
   auto j = R"( {
   "disable_category" : ["c1"],
    "translations": [
     {
       "category": "c1",
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "category": "c1",
       "patterns": [
         "pattern1"
       ],
       "print": "print this message"
     },
     {
       "category": "c2",
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

TEST_CASE("translations count types")
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
       "count": "scoped",
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "count": "global",
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 3);
   CHECK(trs.at(0).count == count_type::none);
   CHECK(trs.at(1).count == count_type::scoped);
   CHECK(trs.at(2).count == count_type::global);
}

TEST_CASE("translations repeat")
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
       "repeat": false,
       "print": "print this message"
     },
     {
       "patterns": [
         "pattern1"
       ],
       "repeat": true,
       "print": "print this message"
     }
    ]
  }
  )"_json;

   JsonConfigParser parser(j);
   parser.load_translations();
   const auto& trs = parser.get_translations();
   CHECK(trs.size() == 3);
   CHECK(trs.at(0).repeat == true);
   CHECK(trs.at(1).repeat == false);
   CHECK(trs.at(2).repeat == true);
}

TEST_CASE("read full configuration")
{
   auto j = R"( {
    "translations": [
     {
       "category": "category_name",
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
       "count": "global"
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
   CHECK(tr.category == "category_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   CHECK(tr.repeat == true);
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.count == count_type::global);
}

TEST_CASE("read minimal mandatory configuration")
{
   auto j = R"( {
    "translations": [
     {
       "category": "category_name",
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
       "count": "global"
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
   CHECK(tr.category == "category_name");
   CHECK(tr.patterns == std::vector<std::string>({"pattern1", "pattern2"}));
   CHECK(tr.print == "print this message");
   CHECK(tr.repeat == true);
   auto variables = tr.variables;
   CHECK(variables.at(0).startswith == "v1startswith");
   CHECK(variables.at(0).endswith == "v1endswith");
   CHECK(variables.at(1).startswith == "v2startswith");
   CHECK(variables.at(1).endswith == "v2endswith");
   CHECK(tr.count == count_type::global);
}

TEST_CASE("translations unavailable")
{
   auto j = R"( { })"_json;
   JsonConfigParser parser(j);
   CHECK_THROWS(parser.load_translations());
}

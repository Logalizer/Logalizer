#include "translator.h"
#include <iostream>
#include "catch.hpp"
#include "configparser_mock.h"

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
   CHECK(line == expected);
}

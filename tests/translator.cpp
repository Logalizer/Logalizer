#include "translator.h"
#include <iostream>
#include "catch.hpp"

using namespace Logalizer::Config;
namespace unit_test {
class TranslatorTester {
  public:
   TranslatorTester(Translator translator) : tr(translator)
   {
   }
   [[nodiscard]] bool is_blacklisted(std::string const &line, std::vector<std::string> const &blacklists)
   {
      return tr.is_blacklisted(line, blacklists);
   }
   [[nodiscard]] bool is_deleted(std::string const &line, std::vector<std::string> const &delete_lines,
                                 std::vector<std::regex> const &delete_lines_regex) noexcept
   {
      return tr.is_deleted(line, delete_lines, delete_lines_regex);
   }
   void replace(std::string *line, std::vector<Logalizer::Config::replacement> const &replacements)
   {
      tr.replace(line, replacements);
   }

  private:
   Translator tr;
};
}  // namespace unit_test

TEST_CASE("is_blacklisted")
{
   unit_test::TranslatorTester tr(Translator{});
   std::vector<std::string> blacklist = {"b1", "b2"};
   std::string line;
   line = "this is a text";
   CHECK_FALSE(tr.is_blacklisted(line, blacklist));
   line = "this is a text with b1";
   CHECK(tr.is_blacklisted(line, blacklist));
   line = "this is a text with b2";
   CHECK(tr.is_blacklisted(line, blacklist));
}

TEST_CASE("is_deleted")
{
   unit_test::TranslatorTester tr(Translator{});
   std::vector<std::string> deleted = {"d1", "d2"};
   std::vector<std::regex> deleted_regex = {std::regex(
       " _r.*x_ ", std::regex_constants::grep | std::regex_constants::nosubs | std::regex_constants::optimize)};
   std::string line;
   line = "this is a text for _regex_ testing";
   CHECK(tr.is_deleted(line, deleted, deleted_regex));
   line = "this is some text with d1";
   CHECK(tr.is_deleted(line, deleted, deleted_regex));
   line = "this is some text with d2";
   CHECK(tr.is_deleted(line, deleted, {}));
   line = "this is some text";
   CHECK_FALSE(tr.is_deleted(line, deleted, deleted_regex));
}

TEST_CASE("replace")
{
   unit_test::TranslatorTester tr(Translator{});
   std::vector<replacement> replacments = {{"s1", "r1"}, {"s2", "r2"}};
   std::string line;
   line = "Text with s1 S1 s2 S2";
   std::string expected = "Text with r1 S1 r2 S2";
   tr.replace(&line, replacments);
   CHECK(line == expected);
}

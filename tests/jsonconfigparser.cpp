#include "jsonconfigparser.h"
#include "catch.hpp"

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
   parser.load_delete_lines();
   CHECK(parser.get_delete_lines() == std::vector<std::string>({}));
   CHECK(parser.get_delete_lines_regex().size() == 0);
}

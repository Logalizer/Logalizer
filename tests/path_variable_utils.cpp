#include "catch.hpp"
#include "path_variable_utils.h"
#include "sys/stat.h"
#include "sys/types.h"

using namespace Logalizer::Config::Utils;

TEST_CASE("dir_file: should return directory and file")
{
   std::string const dir = "/home/test/log";
   std::string const file = "test.log";
   std::string const path = dir + "/" + file;

   auto const [result_dir, result_file] = dir_file(path);
   REQUIRE(result_dir == dir);
   REQUIRE(result_file == file);
}

TEST_CASE("dir_file: should return directory and file for empty path")
{
   auto const [result_dir, result_file] = dir_file("");
   REQUIRE(result_dir == ".");
   REQUIRE(result_file == "");
}

TEST_CASE("dir_file: should return directory and file for file only path")
{
   auto const [result_dir, result_file] = dir_file("test.log");
   REQUIRE(result_dir == ".");
   REQUIRE(result_file == "test.log");
}

TEST_CASE("dir_file: should return directory and file for empty file")
{
   auto const [result_dir, result_file] = dir_file("/var/log/");
   REQUIRE(result_dir == "/var/log");
   REQUIRE(result_file == "");
}
TEST_CASE("dir_file: should return directory and file for /")
{
   auto const [result_dir, result_file] = dir_file("/");
   REQUIRE(result_dir == "/");
   REQUIRE(result_file == "");
}

TEST_CASE("dir_file: should return directory and file :windows")
{
   std::string const dir = "D:\\test";
   std::string const file = "test.log";
   std::string const path = dir + "\\" + file;

   auto const [result_dir, result_file] = dir_file(path);
   REQUIRE(result_dir == dir);
   REQUIRE(result_file == file);
}

TEST_CASE("dir_file: should return directory and file for empty file :windows")
{
   auto const [result_dir, result_file] = dir_file("D:\\test\\");
   REQUIRE(result_dir == "D:\\test");
   REQUIRE(result_file == "");
}

TEST_CASE("dir_file: should return directory and file for / :windows")
{
   auto const [result_dir, result_file] = dir_file("D:\\");
   REQUIRE(result_dir == "D:");
   REQUIRE(result_file == "");
}

TEST_CASE("without_extension: should return base name of file given full path")
{
   std::string const dir = "/home/test/log";
   std::string const file = "test.log";
   std::string const path = dir + "/" + file;

   std::string const basename = without_extension(path);
   REQUIRE(basename == "test");
}

TEST_CASE("without_extension: should return base name of file given only file")
{
   std::string const file = "test.log";

   std::string const basename = without_extension(file);
   REQUIRE(basename == "test");
}

TEST_CASE("without_extension: should return base name of file given full path with no extension")
{
   std::string const dir = "/home/test/log";
   std::string const file = "test";
   std::string const path = dir + "/" + file;

   std::string const basename = without_extension(path);
   REQUIRE(basename == "test");
}

TEST_CASE("without_extension: should return base name of file given only file with no extension")
{
   std::string const file = "test";

   std::string const basename = without_extension(file);
   REQUIRE(basename == "test");
}

TEST_CASE("replace_all: should replace all occurances of search string")
{
   std::string const find = "find";
   std::string const replace = "search";
   std::string phrase = "find and replace: find all occurances and replace them";
   std::string const expected_phrase = "search and replace: search all occurances and replace them";
   replace_all(&phrase, find, replace);
   REQUIRE(phrase == expected_phrase);
}

TEST_CASE("replace_all: should replace single occurance of search string")
{
   std::string const find = "find";
   std::string const replace = "search";
   std::string phrase = "find all occurances and replace them";
   std::string const expected_phrase = "search all occurances and replace them";
   replace_all(&phrase, find, replace);
   REQUIRE(phrase == expected_phrase);
}

TEST_CASE("replace_all: should replace no occurance of search string if found nothing")
{
   std::string const find = "search";
   std::string const replace = "inspect";
   std::string phrase = "find all occurances and replace them";
   std::string const expected_phrase = "find all occurances and replace them";
   replace_all(&phrase, find, replace);
   REQUIRE(phrase == expected_phrase);
}

TEST_CASE("replace_all: should replace all occurace found anywhere")
{
   std::string const find = "find";
   std::string const replace = "search";
   std::string phrase = "findall occurances and replace them\nfinder";
   std::string const expected_phrase = "searchall occurances and replace them\nsearcher";
   replace_all(&phrase, find, replace);
   REQUIRE(phrase == expected_phrase);
}

TEST_CASE("replace_all: should replace all occurace found anywhere even with single length search string")
{
   std::string const find = "f";
   std::string const replace = "F";
   std::string phrase = "findall occurances and replace them\nfinder";
   std::string const expected_phrase = "Findall occurances and replace them\nFinder";
   replace_all(&phrase, find, replace);
   REQUIRE(phrase == expected_phrase);
}

TEST_CASE("is_directory: should return false if file is given")
{
   std::string const path = "/tmp/logalizer_test";
   std::string const remove_cmd = std::string("rm -r ") + path;
   std::string const touch_cmd = std::string("touch ") + path;
   system(touch_cmd.c_str());
   CHECK_FALSE(is_directory(path));
   system(remove_cmd.c_str());
}

TEST_CASE("is_directory: should return false if nothing is given")
{
   std::string const path = "";
   CHECK_FALSE(is_directory(path));
}

TEST_CASE("mkdir: should make directory")
{
   std::string const path = "/tmp/logalizer_test";
   std::string const remove_cmd = std::string("rm -r ") + path;
   CHECK(mkdir(path));
   CHECK(is_directory(path));
   system(remove_cmd.c_str());
}

TEST_CASE("mkdir: should make directory recursively by default")
{
   std::string const path = "/tmp/logalizer_test";
   std::string const recursive_path = path + "/1/2/3";
   std::string const remove_cmd = std::string("rm -r ") + path;
   CHECK(mkdir(recursive_path));
   CHECK(is_directory(recursive_path));
   system(remove_cmd.c_str());
}

TEST_CASE("mkdir: should not fail if directory already exists")
{
   std::string const path = "/tmp/";
   CHECK(mkdir(path));
   CHECK(is_directory(path));
}

TEST_CASE("mkdir: should fail if a file exists with the same name as directory")
{
   std::string const path = "/tmp/logalizer_test";
   std::string const remove_cmd = std::string("rm -r ") + path;
   std::string const touch_cmd = std::string("touch ") + path;
   system(touch_cmd.c_str());
   CHECK_FALSE(is_directory(path));
   CHECK_FALSE(mkdir(path));
   system(remove_cmd.c_str());
}

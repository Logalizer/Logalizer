#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace Logalizer::Config {

/**
 *  It might be needed to extract a substring from a line.
 * This substring will be a vaiable.
 * To capture that variable we look for the surrounding string.
 * Variable structure holds the start string and end string.
 * With this information we can extract the middle variable.
 */
struct variable {
   std::string startswith;
   std::string endswith;
};

enum class duplicates_t { allowed, remove_all, remove_continuous, count_all, count_continuous };

/**
 *  This hold the data populated in "translations" in the config file.
 * This will hold the information to translate a particular line to a different one.
 */
struct translation {
  public:
   std::string category;
   std::vector<std::string> patterns;
   std::string print;
   std::vector<variable> variables;
   duplicates_t duplicates = duplicates_t::allowed;

   [[nodiscard]] bool in(std::string const &line) const
   {
      auto matches = [&line](auto const &pattern) {
         if (line.find(pattern) != std::string::npos)
            return true;
         else
            return false;
      };
      return std::all_of(cbegin(patterns), cend(patterns), matches);
   }
};

struct replacement {
  public:
   std::string search;
   std::string replace;
   replacement(std::string s, std::string r) : search(std::move(s)), replace(std::move(r))
   {
   }
};

struct output_files {
   std::string backup;
   std::string translation;
};

struct path_vars {
   std::string dir;
   std::string file;
   std::string file_no_ext;
};

}  // namespace Logalizer::Config

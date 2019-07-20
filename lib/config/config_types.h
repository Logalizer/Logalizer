#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace Logalizer::Config {

struct variable {
  public:
   std::string startswith;
   std::string endswith;
};

struct translation {
  public:
   std::string category;
   std::vector<std::string> patterns;
   std::string print;
   std::vector<variable> variables;
   bool repeat = true;

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

}  // namespace Logalizer::Config

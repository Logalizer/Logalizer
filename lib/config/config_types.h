#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace Logalizer::Config {

/**
 * @brief variable is used to extract a substring from a line
 *
 * This substring is called a vaiable.
 * To capture variables we look for the surrounding string.
 * The string that preceds the variable text is start string.
 * The string that follows the variable text is end string.
 * Variable structure holds the start string and end string.
 * With this information we can extract the middle content as variable.
 */

struct variable {
   std::string startswith;
   std::string endswith;
};

/**
 * @brief Defines how to handle duplicates in translation
 *
 */
enum class duplicates_t {
   allowed,            /// Allow duplicates, by default all are allowed
   remove,             /// Removes all duplicates in the translation and keeps only the first entry
   remove_continuous,  /// Removes duplicates entries that occurs continuously in the translation. Only continuous
                       /// entries are removed.
   count,            /// Same as remove and additionally helps count duplicates. Updates `${count}` in the first entry.
   count_continuous  /// Same as remove_continuous. Additionally it counts continuously occurring duplicates and updates
                     /// `${count}` in the corresponding entry.
};

/**
 * @brief pairs is used to check if a matching pair is found for a print
 *
 * If a print line is found search for the corresponding pair before a delimiter
 * If not found print error_print string
 */

struct pair {
   std::string source;     /// Defines how to find a print line in translated file
   std::string pairswith;  /// Defines how to find a matching pair line in translated file
   std::string before;     /// Defines how to find a terminator before which the pair has to be found
   std::string error;      /// If a matching pair is not found print this error_print before the terminator
};

/**
 * @brief translation holds all the configuration needed to translate a line
 *
 * This holds the data populated in "translations" in the config file.
 * This will hold the information to translate a particular line to a different one.
 */
struct translation {
   std::string category;
   std::vector<std::string> patterns;
   std::string print;
   std::vector<variable> variables;
   duplicates_t duplicates = duplicates_t::allowed;
   int padding_variable;

   [[nodiscard]] bool in(std::string const& line) const
   {
      auto matches = [&line](auto const& pattern) {
         return static_cast<bool>(line.find(pattern) != std::string::npos);
      };
      return std::all_of(cbegin(patterns), cend(patterns), matches);
   }
};

/**
 * @brief Holds search and replace tokens
 *
 * This will be used to search a token and replace it in the input line
 */
struct replacement {
   std::string search;
   std::string replace;
   replacement(std::string s, std::string r) : search(std::move(s)), replace(std::move(r))
   {
   }
};

/**
 * @brief Holds the path to backup and translation files
 *
 */
struct output_files {
   std::string backup;
   std::string translation;
};

/**
 * @brief Holds the directory, file name and file name without extension of the input file
 *
 */
struct path_vars {
   std::string dir;
   std::string file;
   std::string file_no_ext;
};

}  // namespace Logalizer::Config

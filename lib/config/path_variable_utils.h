#pragma once

#include <iostream>
#include <string>

namespace Logalizer::Config::Utils {

std::pair<std::string, std::string> dir_file(std::string const &basefile);

void replace_all(std::string *input, std::string const &token, std::string const &replace);

void replace_paths(std::string *input, std::string const &dir, std::string const &file);

void mkdir(std::string path);
}  // namespace Logalizer::Config::Utils

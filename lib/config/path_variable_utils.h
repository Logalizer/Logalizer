#pragma once

#include <iostream>
#include <string>

namespace Logalizer::Config::Utils {

std::pair<std::string, std::string> dir_file(std::string const &basefile);

std::string without_extension(std::string const &file);

void replace_all(std::string *input, std::string const &token, std::string const &replace);

void replace_paths(std::string *input, std::string const &dir, std::string const &file);

bool is_directory(std::string const &path);

bool mkdir(std::string const &path);
}  // namespace Logalizer::Config::Utils

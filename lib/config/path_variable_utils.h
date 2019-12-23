#pragma once

#include <iostream>
#include <string>

namespace Logalizer::Config::Utils {

std::pair<std::string, std::string> getDirFile(std::string const &basefile);

void findAndReplace(std::string *input, std::string const &token, std::string const &replace);

void replaceStringVariables(std::string *input, std::string const &dir, std::string const &file);

void mkdir(std::string path);
}  // namespace Logalizer::Config::Utils

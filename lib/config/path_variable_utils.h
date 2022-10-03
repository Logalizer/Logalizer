#pragma once

#include <iostream>
#include <string>

namespace Logalizer::Config::Utils {
/**
 * @brief Get the executable path
 *
 * @return std::string
 */
std::string get_exe_path();

/**
 * @brief Get the executable directory
 *
 * @return std::string
 */
std::string get_exe_dir();

}  // namespace Logalizer::Config::Utils

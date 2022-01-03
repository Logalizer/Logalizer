#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <memory>
#include "LogalizerConfig.h"
#include "jsonconfigparser.h"
#include "translator.hpp"

void printHelp()
{
   std::cout << "\n"
                "Logalizer\n"
                "  Filter and translate a trace log to a meaningful format.\n"
                "  Logesh Gopalakrishnan\n\n"
                "Usage:\n"
                "  logalizer -c <config> -f <log>\n"
                "  logalizer -f <log>\n"
                "  logalizer -h | --help\n"
                "  logalizer --config-help\n"
                "  logalizer --version\n"
                "\n"
                "Options:\n"
                "  -h --help        Show this screen\n"
                "  --config-help    Show sample configuration\n"
                "  --version        Show version\n"
                "  -c <config>      Translation configuration file. Default is ./config.json\n"
                "  -f <log>         Log file to be interpreted\n"
                "\n"
                "Example:\n"
                "  logalizer -c config.json -f trace.log\n"
                "  logalizer -f trace.log\n"
             << std::endl;
}

void printConfigHelp()
{
   std::cout <<
       R"(
{
  "sample log": [
    "2019-07-13 14:29:05:147 [Networking] DisplayClient say_hello: asking to connect to server",
    "2017-07-13 14:29:06:239 [Test] note: Delete this line from the trace",
    "2017-07-13 14:29:06:239 [Test] note: regex lines are very slow",
    "2019-07-13 14:29:06:239 [Networking] DisplayClient server_response: Today's is a hot day with a max temperature of 37C possibly",
    "2019-07-13 14:29:06:239 [Networking] DisplayClient start_routine: state=waiting, entering locked state"
  ],
  "translations": [
    {
      "category": "Networking",
      "patterns": [
        "say_hello"
      ],
      "print": "client -> server : sayHello",
      "variables": []
    },
    {
      "category": "Networking",
      "patterns": [
        "start_routine"
      ],
      "print": "client -> server : start_routine",
      "variables": [
        {
          "startswith": "state=",
          "endswith": ", entering"
        }
      ]
    },
    {
      "category": "Networking",
      "patterns": [
        "Today",
        "temperature"
      ],
      "print": "server -> client : Today is ${1} and ${2}",
      "variables": [
        {
          "startswith": "is a ",
          "endswith": " day"
        },
        {
          "startswith": "of ",
          "endswith": " possibly"
        }
      ]
    }
  ],
  "disable_category": [
    "exclude a particular category of translations"
  ],
  "wrap_text_pre": [
    "@startuml",
    "skinparam dpi 300"
  ],
  "wrap_text_post": [
    "== Done ==",
    "@enduml"
  ],
  "blacklist": [
    "Do not translate this line",
    "and this line too"
  ],
  "delete_lines": [
    "Delete this line from the trace",
    "2017.* regex lines are very slow"
  ],
  "replace_words": {
    "search": "replace"
  },
  "execute": [
    "java -DPLANTUML_LIMIT_SIZE=32768 -jar plantuml.jar \"${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt\""
  ],
  "backup_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}.original",
  "translation_file": "${fileDirname}/${fileBasenameNoExtension}/${fileBasename}_seq.txt",
  "auto_new_line": true
}
)";
}

using namespace Logalizer::Config;

struct CMD_Args {
   const std::string config_file;
   const std::string log_file;
};

CMD_Args parse_cmd_line(const std::vector<std::string_view> &args)
{
   std::string log_file, config_file;
   for (auto it = cbegin(args), endit = cend(args); it != endit; ++it) {
      if ((*it == "-f" || *it == "--file") && next(it) != endit) {
         log_file = *(next(it));
      }
      else if ((*it == "-c" || *it == "--config") && next(it) != endit) {
         config_file = *(next(it));
      }
      else if (*it == "-h" || *it == "--help") {
         printHelp();
         exit(0);
      }
      else if (*it == "--version") {
         std::cout << "Logalizer v" << LOGALIZER_VERSION_MAJOR << "." << LOGALIZER_VERSION_MINOR << std::endl;
         exit(0);
      }
      else if (*it == "--config-help") {
         printConfigHelp();
         exit(0);
      }
   }
   if (log_file.empty()) {
      printHelp();
      exit(0);
   }
   const auto [dir, dummy] = Utils::dir_file(args.at(0).data());
   if (chdir(dir.c_str())) std::cerr << "Could not change directory to " << dir;
   if (config_file.empty()) config_file = "config.json";
   if (struct stat my_stat; stat(config_file.c_str(), &my_stat) != 0) {
      std::cerr << config_file << " config file not available\n\n";
      printHelp();
      exit(1);
   }
   if (struct stat my_stat; stat(log_file.c_str(), &my_stat) != 0) {
      std::cerr << log_file << " log file not available\n";
      exit(1);
   }

   return {config_file, log_file};
}

void backup_if_not_exists(const std::string &original, const std::string &backup)
{
   if (original.empty() || backup.empty()) return;

   Utils::mkdir(Utils::dir_file(backup).first);
   if (struct stat my_stat; stat(backup.c_str(), &my_stat) != 0) {
      std::ifstream src(original, std::ios::binary);
      std::ofstream dst(backup, std::ios::binary);
      dst << src.rdbuf();
   }
}

static std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
static std::chrono::time_point<std::chrono::high_resolution_clock> end_time;

void start_benchmark()
{
   start_time = std::chrono::high_resolution_clock::now();
}

void end_benchmark(std::string const &print)
{
   end_time = std::chrono::high_resolution_clock::now();
   auto count = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
   std::cout << '[' << count << "ms] " << print << '\n';
}

int main(int argc, char **argv)
{
   const std::vector<std::string_view> args(argv, argv + argc);
   const CMD_Args cmd_args = parse_cmd_line(args);

   start_benchmark();
   JsonConfigParser p(cmd_args.config_file);
   p.read_config_file();
   p.load_configurations();
   p.update_relative_paths(cmd_args.log_file);
   end_benchmark("Configuration loaded");

   backup_if_not_exists(cmd_args.log_file, p.get_backup_file());

   start_benchmark();
   translate_file(cmd_args.log_file, p.get_translation_file(), p);
   end_benchmark("Translation file generated");

   for (auto const &command : p.get_execute_commands()) {
      std::cout << "Executing...\n";
      start_benchmark();
      const char *command_str = command.c_str();
      std::cout << command_str << std::endl;
      if (const int returnval = system(command_str)) {
         std::cerr << TAG_EXECUTE << " : " << command << " execution failed\n";
         return returnval;
      }
      end_benchmark("Executed");
   }

   return 0;
}

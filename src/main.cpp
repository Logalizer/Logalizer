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
                "  --config-help    Show version\n"
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
  "generate_uml": [
    "java",
    "-DPLANTUML_LIMIT_SIZE=16384",
    "-jar",
    "plantuml.jar",
    "-config",
    "uml_config.txt",
    "\"${fileDirname}/${fileBasenameNoExtension}_seq.txt\""
  ],
  "backup_file": "${fileDirname}/${fileBasenameNoExtension}.original",
  "translated_uml_file": "${fileDirname}/${fileBasenameNoExtension}_seq.txt"
}
)";
}

std::pair<std::string, std::string> getCmdLineArgs(std::vector<std::string_view> &args)
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
   auto dirfile = Logalizer::Config::Utils::getDirFile(args.at(0).data());
   if (chdir(dirfile.first.c_str())) std::cerr << "Could not change directory to " << dirfile.first;
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

   return make_pair(config_file, log_file);
}

void backupIfNotExists(std::string original, std::string backup)
{
   if (original.empty() || backup.empty()) return;

   if (struct stat my_stat; stat(backup.c_str(), &my_stat) != 0) {
      std::ifstream src(original, std::ios::binary);
      std::ofstream dst(backup, std::ios::binary);
      dst << src.rdbuf();
   }
}

int main(int argc, char **argv)
{
   std::vector<std::string_view> args(argv, argv + argc);
   std::pair<std::string, std::string> files = getCmdLineArgs(args);
   std::string config_file = files.first;
   std::string log_file = files.second;

   auto start = std::chrono::high_resolution_clock::now();
   std::unique_ptr<Logalizer::Config::ConfigParser> p =
       std::make_unique<Logalizer::Config::JsonConfigParser>(config_file);
   p->loadConfigFile();
   p->loadAllConfigurations();
   auto end = std::chrono::high_resolution_clock::now();
   std::cout << "loading configuration took "
             << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us\n";
   p->updateRelativePaths(log_file);

   backupIfNotExists(log_file, p->getBackupFile());

   createTranslationFile(log_file, p->getUmlFile(), p.get());

   std::cout << p->getGenerateUmlCommand().c_str() << std::endl;
   if (int returnval = system(p->getGenerateUmlCommand().c_str())) {
      std::cerr << TAG_GENERATE_UML << " : " << p->getGenerateUmlCommand().c_str() << " execution failed\n";
      return returnval;
   }

   return 0;
}

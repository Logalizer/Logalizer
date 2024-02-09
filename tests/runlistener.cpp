#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

class testRunListener : public Catch::EventListenerBase {
  public:
   using Catch::EventListenerBase::EventListenerBase;
   void spdlogStdout()
   {
      // create color multi threaded logger
      auto console = spdlog::stdout_color_mt("console");
      auto err_logger = spdlog::stderr_color_mt("stderr");
      spdlog::get("console")->info(
          "loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
   }
   void testRunStarting(Catch::TestRunInfo const&) override
   {
      spdlogStdout();
      spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
      spdlog::debug("Set global log level to debug");
   }
};

CATCH_REGISTER_LISTENER(testRunListener)
//! [code]

#include "Model.h"
#include "View.h"
#include "Controller.h"
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

// Main method of Spirograph program.
// Creates a model, view and controller. Controller.run() is called.
int main()
{
    // DEBUGGING NOTES (NOT RELEVANT TO PROGRAM)
    spdlog::cfg::load_env_levels();  // Need to do this only once in main()
    // Logging Demo
    // Template: export SPDLOG_LEVEL=xxx
    spdlog::info("This is at INFO Level,");
    spdlog::info("Some INFO message with arg: {}", 1234);
    spdlog::info("Some INFO message with multiple args: {0} and {1}", "hello", 1234);
    spdlog::error("This is at ERROR Leve,");
    spdlog::debug("This is at DEBUG Level,");

    // BEGIN PROGRAM
    Model model;
    View view;
    Controller controller(&model,&view);
    controller.run();
}

//! [code]

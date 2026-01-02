#include "cli.hpp"
#include <cstdio>
#include <string>

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("advent-of-code", "Advent of Code Solutions");

  // Year 2024 commands
  executor.add_nested_command(
      "year-2024.day-1.part-1", "Day 1 Part 1: Trebuchet?!",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2024, Day 1, Part 1\n");

        bool verbose = result.get_bool("--verbose");
        auto input_files = result.get_args("--input");

        if (verbose) {
          std::printf("Verbose mode enabled\n");
        }

        if (!input_files.empty()) {
          std::printf("Input file: %s\n", input_files[0].c_str());
        } else {
          std::printf("Using default input\n");
        }

        // Simulate solution
        std::printf("Result: 54159\n");
        return 0;
      });

  executor.add_nested_command(
      "year-2024.day-1.part-2", "Day 1 Part 2: Trebuchet?! (Part 2)",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2024, Day 1, Part 2\n");

        bool verbose = result.get_bool("--verbose");
        if (verbose) {
          std::printf("Verbose mode enabled\n");
        }

        std::printf("Result: 53866\n");
        return 0;
      });

  executor.add_nested_command(
      "year-2024.day-2.part-1", "Day 2 Part 1: Cube Conundrum",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2024, Day 2, Part 1\n");
        std::printf("Result: 2283\n");
        return 0;
      });

  // Year 2025 commands
  executor.add_nested_command(
      "year-2025.day-1.part-1", "Day 1 Part 1: Historian Hysteria",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2025, Day 1, Part 1\n");

        auto positional = result.positional_args;
        if (!positional.empty()) {
          std::printf("Processing with args: ");
          for (const auto &arg : positional) {
            std::printf("%s ", arg.c_str());
          }
          std::printf("\n");
        }

        std::printf("Result: 2264607\n");
        return 0;
      });

  executor.add_nested_command(
      "year-2025.day-1.part-2", "Day 1 Part 2: Historian Hysteria (Part 2)",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2025, Day 1, Part 2\n");
        std::printf("Result: 19457120\n");
        return 0;
      });

  executor.add_nested_command(
      "year-2025.day-2.part-1", "Day 2 Part 1: Red-Nosed Reports",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2025, Day 2, Part 1\n");

        bool test_mode = result.get_bool("--test");
        if (test_mode) {
          std::printf("Running with test input\n");
          std::printf("Result: 2\n");
        } else {
          std::printf("Result: 564\n");
        }
        return 0;
      });

  executor.add_nested_command(
      "year-2025.day-2.part-2", "Day 2 Part 2: Red-Nosed Reports (Part 2)",
      [](const cli::ParseResult &result) {
        std::printf("Running Advent of Code 2025, Day 2, Part 2\n");
        std::printf("Result: 604\n");
        return 0;
      });

  // Add some command-specific flags
  executor.add_nested_command_flag("year-2024.day-1.part-1", "-i,--input",
                                   cli::FlagType::MultiArg, "Input file path");

  executor.add_nested_command_flag("year-2025.day-2.part-1", "-t,--test",
                                   cli::FlagType::Boolean, "Use test input");

  executor.add_nested_command_flag("year-2025.day-2.part-2", "-t,--test",
                                   cli::FlagType::Boolean, "Use test input");

  // Add global verbose flag
  executor.add_flag("-v,--verbose", cli::FlagType::Boolean,
                    "Enable verbose output");

  return executor.run(argc, argv);
}

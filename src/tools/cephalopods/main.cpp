#include <cstdio>

#include "cephalopods.hpp"
#include "cli.hpp"
#include "stdin_reader.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("cephalopods", "Cephalopod-related utilities");

  executor.add_command(
      "do-homework", "Process homework from stdin",
      [](const cli::ParseResult & /* result */) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(
              stderr,
              "Error: No input provided. Pipe homework lines to stdin.\n");
          return 1;
        }

        auto lines = cli::StdinReader::read_lines();
        auto result = cephalopods::do_homework(lines);

        if (!result.has_value()) {
          std::fprintf(stderr, "Error: Failed to solve homework.\n");
          return 1;
        }

        std::printf("%lld\n", result.value());
        return 0;
      });

  executor.add_command(
      "do-homework-true", "Process homework from stdin (verbose mode)",
      [](const cli::ParseResult &result) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(
              stderr,
              "Error: No input provided. Pipe homework lines to stdin.\n");
          return 1;
        }

        bool verbose = result.get_bool("--verbose");
        auto lines = cli::StdinReader::read_lines();
        auto homework_solution = cephalopods::do_homework_true(lines, verbose);

        if (!homework_solution.has_value()) {
          std::fprintf(stderr, "Error: Failed to solve homework.\n");
          return 1;
        }

        std::printf("%lld\n", homework_solution.value());
        return 0;
      });

  executor.add_command_flag("do-homework-true", "-v,--verbose",
                            cli::FlagType::Boolean, "Enable verbose output");

  return executor.run(argc, argv);
}

#include <cstdio>

#include "cli.hpp"
#include "printing.hpp"
#include "stdin_reader.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("printing", "Print formatted output");

  // Default command - print input
  executor.add_command(
      "print", "Print formatted output from input",
      [](const cli::ParseResult &result) {
        bool verbose = result.get_bool("--verbose");

        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin\n");
          return 1;
        }

        if (verbose) {
          std::fprintf(stderr, "Reading from stdin...\n");
        }

        bool count = result.get_bool("--count");
        if (count) {
          auto lines = cli::StdinReader::read_lines();
          const auto how_many_with_4_neighbors_or_less =
              printing::how_many_less_than_4_neighbors(lines);
          std::printf("%d\n", how_many_with_4_neighbors_or_less);
        }
        return 0;
      });

  executor.add_command_flag("print", "-c,--count", cli::FlagType::Boolean,
                            "Count the number of @ with 4 or less neighbors");

  // Total can remove command
  executor.add_command(
      "total-can-remove",
      "Calculate total cells that can be removed iteratively",
      [](const cli::ParseResult &result) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin\n");
          return 1;
        }

        auto lines = cli::StdinReader::read_lines();
        int max_iterations = printing::kMaxIterations;
        auto max_iter_args = result.get_args("--max-iterations");
        if (!max_iter_args.empty()) {
          max_iterations = std::stoi(max_iter_args[0]);
        }

        bool verbose = result.get_bool("--verbose");
        const auto total =
            printing::total_can_remove(lines, max_iterations, verbose);
        std::printf("%lld\n", total);
        return 0;
      });

  executor.add_command_flag("total-can-remove", "-m,--max-iterations",
                            cli::FlagType::MultiArg,
                            "Maximum iterations (default: 1000)");
  executor.add_command_flag("total-can-remove", "-v,--verbose",
                            cli::FlagType::Boolean,
                            "Print grid and can-remove map at each iteration");

  return executor.run(argc, argv);
}

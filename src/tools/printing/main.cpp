#include <cstdio>

#include "cli.hpp"
#include "printing.hpp"
#include "stdin_reader.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("printing", "Print formatted output");

  // "total-can-remove" command - calculate total cells that can be removed
  // iteratively
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

  return executor.run(argc, argv);
}

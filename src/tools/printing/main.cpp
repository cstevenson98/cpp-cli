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

        auto lines = cli::StdinReader::read_lines();

        // const auto number_4_neighbors = printing::count_4_neighbors(lines);
        return 0;
      });

  return executor.run(argc, argv);
}

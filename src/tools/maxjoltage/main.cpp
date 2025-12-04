#include <cstdio>

#include "cli.hpp"
#include "maxjoltage.hpp"
#include "stdin_reader.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("maxjoltage", "Compute maximum joltage from input");

  // Default command - compute max joltage
  executor.add_command(
      "compute", "Compute max joltage from input",
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

        int total_maxjoltage = 0;
        for (const auto &line : lines) {
          int maxjoltage = maxjoltage::maximum_joltage_line_width_2(line);
          total_maxjoltage += maxjoltage;
        }
        std::printf("%d\n", total_maxjoltage);
        return 0;
      });

  return executor.run(argc, argv);
}

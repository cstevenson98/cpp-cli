#include <cstdio>

#include "cli.hpp"
#include "maxjoltage.hpp"
#include "stdin_reader.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("maxjoltage", "Compute maximum joltage from input");

  executor.add_command(
      "compute", "Compute max joltage from input",
      [](const cli::ParseResult &result) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin\n");
          return 1;
        }

        // Get the width parameter (default to 2)
        int width = 2;
        if (result.has_flag("--width")) {
          auto width_args = result.get_args("--width");
          if (!width_args.empty()) {
            try {
              width = std::stoi(width_args[0]);
            } catch (const std::exception &e) {
              std::fprintf(stderr, "Error: Invalid width value '%s'\n",
                           width_args[0].c_str());
              return 1;
            }
          }
        }

        auto lines = cli::StdinReader::read_lines();

        long long total_maxjoltage = 0;
        for (const auto &line : lines) {
          long long maxjoltage = maxjoltage::maximum_joltage(line, width);
          total_maxjoltage += maxjoltage;
        }
        std::printf("%lld\n", total_maxjoltage);
        return 0;
      });

  executor.add_command_flag(
      "compute", "-w,--width", cli::FlagType::MultiArg,
      "Width for maximum joltage computation (default: 2)", false);

  return executor.run(argc, argv);
}

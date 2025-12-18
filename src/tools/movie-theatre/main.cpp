#include <iostream>
#include <string>
#include <vector>

#include "cli.hpp"
#include "movie_theatre.hpp"
#include "stdin_reader.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("movie-theatre",
                            "Optimise movie theatre seating arrangements");

  executor.add_command(
      "maximise", "Maximise theatre seating efficiency",
      [](const cli::ParseResult &result) {
        bool verbose = result.has_flag("verbose") || result.has_flag("v");

        if (!cli::StdinReader::has_piped_input()) {
          std::cerr << "Error: No input provided. Pipe data to stdin.\n";
          std::cerr << "Example: cat theatre.txt | movie-theatre maximise\n";
          return 1;
        }

        // Read all lines from stdin
        auto lines = cli::StdinReader::read_lines();

        if (lines.empty()) {
          std::cerr << "Error: No data to process.\n";
          return 1;
        }

        if (verbose) {
          std::cout << "Processing " << lines.size() << " lines...\n";
        }

        // Create theatre instance and run maximise
        movie_theatre::MovieTheatre theatre(lines);

        if (!theatre.validate_input()) {
          std::cerr << "Error: Invalid input data.\n";
          return 1;
        }

        auto maximise_result = theatre.maximise(verbose);

        if (verbose) {
          std::cout << "Optimisation complete.\n";
          std::cout << "Optimised seats: " << maximise_result << "\n";
        } else {
          std::cout << maximise_result << "\n";
        }

        return 0;
      });

  executor.add_command_flag("maximise", "-v,--verbose", cli::FlagType::Boolean,
                            "Enable verbose output", false);

  return executor.run(argc, argv);
}

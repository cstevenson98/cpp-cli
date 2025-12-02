#include <cstdio>
#include <cstdlib>

#include "cli.hpp"
#include "stdin_reader.hpp"
#include "tail.hpp"

int main(int argc, char* argv[]) {
    cli::CliExecutor executor("tail", "Display the last lines of input");

    // Default command - show last lines
    executor.add_command(
        "show", "Show last N lines of input",
        [](const cli::ParseResult& result) {
            // Get number of lines (default 10)
            std::size_t num_lines = 10;
            auto n_args = result.get_args("--lines");
            if (!n_args.empty()) {
                char* end;
                long val = std::strtol(n_args[0].c_str(), &end, 10);
                if (*end != '\0' || val <= 0) {
                    std::fprintf(stderr, "Error: Invalid line count: %s\n",
                                 n_args[0].c_str());
                    return 1;
                }
                num_lines = static_cast<std::size_t>(val);
            }

            // Get input source
            auto file_args = result.get_args("--file");
            bool verbose = result.get_bool("--verbose");

            tail::TailResult tail_result;

            if (!file_args.empty() && file_args[0] != "-") {
                // Read from file
                if (verbose) {
                    std::fprintf(stderr, "Reading from file: %s\n",
                                 file_args[0].c_str());
                }
                tail_result = tail::tail_file(file_args[0], num_lines);
            } else if (cli::StdinReader::has_piped_input()) {
                // Read from stdin
                if (verbose) {
                    std::fprintf(stderr, "Reading from stdin...\n");
                }
                auto lines = cli::StdinReader::read_last_n_lines(num_lines);
                tail_result.success = true;
                tail_result.lines = std::move(lines);
            } else {
                std::fprintf(stderr,
                             "Error: No input provided. Pipe data or use -f/--file\n");
                return 1;
            }

            if (!tail_result.success) {
                std::fprintf(stderr, "Error: %s\n",
                             tail_result.error_message.c_str());
                return 1;
            }

            // Output the lines
            for (const auto& line : tail_result.lines) {
                std::printf("%s\n", line.c_str());
            }

            if (verbose) {
                std::fprintf(stderr, "Displayed %zu lines\n",
                             tail_result.lines.size());
            }

            return 0;
        });

    // Add command-specific flags
    executor.add_command_flag("show", "-n,--lines", cli::FlagType::MultiArg,
                              "Number of lines to display (default: 10)");
    executor.add_command_flag("show", "-f,--file", cli::FlagType::MultiArg,
                              "Input file (use - for stdin)");

    // Add global verbose flag
    executor.add_flag("-v,--verbose", cli::FlagType::Boolean,
                      "Enable verbose output");

    return executor.run(argc, argv);
}


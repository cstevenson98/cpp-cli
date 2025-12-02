#include <cstdio>

#include "cli.hpp"
#include "cp_file.hpp"

int main(int argc, char* argv[]) {
  cli::CliExecutor executor("cp_file", "Copy files from source to destination");

  // Add copy command
  executor.add_command(
      "copy", "Copy a file", [](const cli::ParseResult& result) {
        auto sources = result.get_args("--source");
        auto dests = result.get_args("--dest");
        bool force = result.get_bool("--force");
        bool verbose = result.get_bool("--verbose");

        if (sources.empty()) {
          std::fprintf(stderr, "Error: No source file specified\n");
          return 1;
        }

        if (dests.empty()) {
          std::fprintf(stderr, "Error: No destination specified\n");
          return 1;
        }

        const std::string& source = sources[0];
        const std::string& dest = dests[0];

        if (verbose) {
          std::printf("Copying '%s' to '%s'...\n", source.c_str(),
                      dest.c_str());
        }

        auto copy_result = cp_file::copy_file(source, dest, force);

        if (!copy_result.success) {
          std::fprintf(stderr, "Error: %s\n",
                       copy_result.error_message.c_str());
          return 1;
        }

        if (verbose) {
          std::printf("Copied %zu bytes\n", copy_result.bytes_copied);
        }

        return 0;
      });

  // Add command-specific flags
  executor.add_command_flag("copy", "-s,--source", cli::FlagType::MultiArg,
                            "Source file path", true);
  executor.add_command_flag("copy", "-d,--dest", cli::FlagType::MultiArg,
                            "Destination file path", true);
  executor.add_command_flag("copy", "-f,--force", cli::FlagType::Boolean,
                            "Overwrite existing files");

  // Add global verbose flag
  executor.add_flag("-v,--verbose", cli::FlagType::Boolean,
                    "Enable verbose output");

  return executor.run(argc, argv);
}

#include <cstdio>

#include "cli.hpp"
#include "cp_file.hpp"

int main(int argc, char* argv[]) {
  cli::CliExecutor executor("cp_file", "Copy files from source to destination");
  executor.set_usage("<source> <dest> [options]");

  executor.add_flag("-f,--force", cli::FlagType::Boolean,
                    "Overwrite existing files");
  executor.add_flag("-v,--verbose", cli::FlagType::Boolean,
                    "Enable verbose output");

  executor.set_handler([](const cli::ParseResult& result) {
    if (result.positional_args.size() < 2) {
      std::fprintf(stderr, "Error: Expected <source> <dest>\n");
      std::fprintf(stderr, "Use --help for usage information.\n");
      return 1;
    }

    const std::string& source = result.positional_args[0];
    const std::string& dest = result.positional_args[1];
    bool force = result.get_bool("--force");
    bool verbose = result.get_bool("--verbose");

    if (verbose) {
      std::printf("Copying '%s' to '%s'...\n", source.c_str(), dest.c_str());
    }

    auto copy_result = cp_file::copy_file(source, dest, force);

    if (!copy_result.success) {
      std::fprintf(stderr, "Error: %s\n", copy_result.error_message.c_str());
      return 1;
    }

    if (verbose) {
      std::printf("Copied %zu bytes\n", copy_result.bytes_copied);
    }

    return 0;
  });

  return executor.run(argc, argv);
}

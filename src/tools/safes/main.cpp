#include <cstdio>

#include "cli.hpp"
#include "safes.hpp"
#include "stdin_reader.hpp"

namespace {

struct SafeParams {
  int initial_position = 0;
  int num_digits = 2;
  bool valid = true;
};

SafeParams parse_safe_params(const cli::ParseResult &result) {
  SafeParams params;

  auto initial_pos_args = result.get_args("--initial-position");
  if (!initial_pos_args.empty()) {
    try {
      params.initial_position = std::stoi(initial_pos_args[0]);
    } catch (const std::exception &e) {
      std::fprintf(stderr, "Error: Invalid initial-position value '%s'\n",
                   initial_pos_args[0].c_str());
      params.valid = false;
      return params;
    }
  }

  auto num_digits_args = result.get_args("--num-digits");
  if (!num_digits_args.empty()) {
    try {
      params.num_digits = std::stoi(num_digits_args[0]);
    } catch (const std::exception &e) {
      std::fprintf(stderr, "Error: Invalid num-digits value '%s'\n",
                   num_digits_args[0].c_str());
      params.valid = false;
      return params;
    }
  }

  return params;
}

} // namespace

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("safes", "Safe cracking utilities");

  // "positions" command - print position after each instruction
  executor.add_command(
      "positions",
      "Print safe position after each instruction (including initial)",
      [](const cli::ParseResult &result) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin\n");
          return 1;
        }

        auto params = parse_safe_params(result);
        if (!params.valid) {
          return 1;
        }

        auto instructions_strs = cli::StdinReader::read_lines();
        auto positions = safes::get_positions(
            instructions_strs, params.initial_position, params.num_digits);
        for (const auto &pos : positions) {
          std::printf("%d\n", pos);
        }
        return 0;
      });

  executor.add_command_flag("positions", "-p,--initial-position",
                            cli::FlagType::MultiArg,
                            "Initial position of the safe (default: 0)");

  executor.add_command_flag("positions", "-d,--num-digits",
                            cli::FlagType::MultiArg,
                            "Number of digits on the safe (default: 2)");

  // "crack" command - count zeros in positions (the password)
  executor.add_command(
      "crack", "Crack safe by counting zeros in positions",
      [](const cli::ParseResult &result) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin\n");
          return 1;
        }

        auto params = parse_safe_params(result);
        if (!params.valid) {
          return 1;
        }

        auto instructions_strs = cli::StdinReader::read_lines();
        const auto password = safes::crack(
            instructions_strs, params.initial_position, params.num_digits);
        std::printf("%lld\n", password);
        return 0;
      });

  executor.add_command_flag("crack", "-p,--initial-position",
                            cli::FlagType::MultiArg,
                            "Initial position of the safe (default: 0)");

  executor.add_command_flag("crack", "-d,--num-digits", cli::FlagType::MultiArg,
                            "Number of digits on the safe (default: 2)");

  // "crack-true" command - count every zero crossing (not just final position)
  executor.add_command(
      "crack-true", "Crack safe by counting every zero crossing",
      [](const cli::ParseResult &result) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin\n");
          return 1;
        }

        auto params = parse_safe_params(result);
        if (!params.valid) {
          return 1;
        }

        auto instructions_strs = cli::StdinReader::read_lines();
        const auto password = safes::crack_true(
            instructions_strs, params.initial_position, params.num_digits);
        std::printf("%lld\n", password);
        return 0;
      });

  executor.add_command_flag("crack-true", "-p,--initial-position",
                            cli::FlagType::MultiArg,
                            "Initial position of the safe (default: 0)");

  executor.add_command_flag("crack-true", "-d,--num-digits",
                            cli::FlagType::MultiArg,
                            "Number of digits on the safe (default: 2)");

  return executor.run(argc, argv);
}

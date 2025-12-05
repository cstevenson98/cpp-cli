#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace cli {

/// Type of flag: boolean (no arguments) or multi-argument
enum class FlagType {
  Boolean,  // Flag is present or not (e.g., -v, --verbose)
  MultiArg  // Flag takes one or more arguments (e.g., -f file1 file2)
};

/// Parsed value for a flag
using FlagValue = std::variant<bool, std::vector<std::string>>;

/// Result of parsing command line arguments
struct ParseResult {
  bool success = false;
  std::string error_message;
  std::string command;
  std::map<std::string, FlagValue> flags;
  std::vector<std::string> positional_args;

  /// Get boolean flag value (returns false if not set or wrong type)
  bool get_bool(const std::string& flag_name) const;

  /// Get multi-arg flag values (returns empty vector if not set or wrong type)
  std::vector<std::string> get_args(const std::string& flag_name) const;

  /// Check if a flag was provided
  bool has_flag(const std::string& flag_name) const;
};

/// Flag definition
struct FlagDef {
  std::string short_name;  // e.g., "-v"
  std::string long_name;   // e.g., "--verbose"
  FlagType type;
  std::string description;
  bool required = false;
};

/// Command definition with callback
using CommandCallback = std::function<int(const ParseResult&)>;

struct CommandDef {
  std::string name;
  std::string description;
  CommandCallback callback;
  std::vector<FlagDef> flags;
};

/// CLI Executor - main class for parsing and executing commands
class CliExecutor {
 public:
  CliExecutor(std::string program_name, std::string description = "");

  /// Set usage string for help (e.g., "<source> <dest>")
  void set_usage(const std::string& usage);

  /// Add a global flag (available to all commands)
  /// @param names Comma-separated short and long names, e.g., "-v,--verbose"
  /// @param type Flag type (Boolean or MultiArg)
  /// @param description Help text for the flag
  /// @param required Whether the flag is required
  void add_flag(const std::string& names, FlagType type,
                const std::string& description = "", bool required = false);

  /// Set handler for command-less mode (just positional args + flags)
  /// @param callback Function to execute with parsed args
  void set_handler(CommandCallback callback);

  /// Add a command (for multi-command CLIs)
  /// @param name Command name
  /// @param description Help text for the command
  /// @param callback Function to execute when command is invoked
  void add_command(const std::string& name, const std::string& description,
                   CommandCallback callback);

  /// Add a flag specific to a command
  void add_command_flag(const std::string& command_name,
                        const std::string& names, FlagType type,
                        const std::string& description = "",
                        bool required = false);

  /// Parse command line arguments
  /// @param argc Argument count
  /// @param argv Argument values
  /// @return ParseResult with parsed values or error
  ParseResult parse(int argc, char* argv[]) const;

  /// Parse from vector of strings (useful for testing)
  ParseResult parse(const std::vector<std::string>& args) const;

  /// Execute the parsed command
  /// @return Exit code from command callback, or -1 on error
  int execute(const ParseResult& result) const;

  /// Parse and execute in one call
  int run(int argc, char* argv[]) const;

  /// Generate help text
  std::string help() const;

  /// Generate help text for a specific command
  std::string help(const std::string& command_name) const;

 private:
  std::string program_name_;
  std::string description_;
  std::string usage_;
  std::vector<FlagDef> global_flags_;
  std::map<std::string, CommandDef> commands_;
  CommandCallback default_handler_;

  /// Parse flag names string into short and long names
  static std::pair<std::string, std::string> parse_flag_names(
      const std::string& names);

  /// Find flag definition by name (short or long)
  const FlagDef* find_flag(const std::string& name,
                           const std::vector<FlagDef>& command_flags) const;

  /// Get canonical name for a flag (prefers long name)
  static std::string canonical_name(const FlagDef& flag);
};

}  // namespace cli

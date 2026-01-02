# Nested Commands Example

This example demonstrates the nested command functionality of the CLI library.

## Overview

The CLI library now supports arbitrarily nested commands, allowing you to create complex command hierarchies like:

```bash
advent-of-code year-2025 day-1 part-1 [args]
kubectl get pods --namespace default
docker container ls --all
```

## Building and Running

```bash
# Build the example
cmake --build build/Debug --target advent-of-code

# Run the example
./build/Debug/src/tools/advent-of-code/advent-of-code --help
```

## Example Usage

### View top-level commands
```bash
$ ./advent-of-code --help
advent-of-code - Advent of Code Solutions

Usage: advent-of-code <command> [options]

Commands:
  year-2024
  year-2025

Options:
  -h, --help	Show help message
  -v, --verbose	Enable verbose output
```

### View nested subcommands
```bash
$ ./advent-of-code year-2025 --help
advent-of-code year-2025

Usage: advent-of-code year-2025 <subcommand> [options]

Subcommands:
  day-1
  day-2

Global Options:
  -h, --help	Show help message
  -v, --verbose	Enable verbose output
```

### View deeper nesting
```bash
$ ./advent-of-code year-2025 day-1 --help
advent-of-code year-2025 day-1

Usage: advent-of-code year-2025 day-1 <subcommand> [options]

Subcommands:
  part-1	Day 1 Part 1: Historian Hysteria
  part-2	Day 1 Part 2: Historian Hysteria (Part 2)

Global Options:
  -h, --help	Show help message
  -v, --verbose	Enable verbose output
```

### Execute a nested command
```bash
$ ./advent-of-code year-2025 day-1 part-1
Running Advent of Code 2025, Day 1, Part 1
Result: 2264607
```

### With flags and arguments
```bash
$ ./advent-of-code year-2025 day-1 part-1 --verbose arg1 arg2
Running Advent of Code 2025, Day 1, Part 1
Processing with args: arg1 arg2 
Result: 2264607
```

## Code Example

```cpp
#include "cli.hpp"

int main(int argc, char* argv[]) {
    cli::CliExecutor executor("advent-of-code", "Advent of Code Solutions");

    // Add nested commands using dot-separated paths
    executor.add_nested_command(
        "year-2025.day-1.part-1",
        "Day 1 Part 1: Historian Hysteria",
        [](const cli::ParseResult& result) {
            std::printf("Running Advent of Code 2025, Day 1, Part 1\n");
            std::printf("Result: 2264607\n");
            return 0;
        });

    // Add command-specific flags
    executor.add_nested_command_flag(
        "year-2025.day-1.part-1",
        "-i,--input",
        cli::FlagType::MultiArg,
        "Input file path");

    // Add global flags
    executor.add_flag("-v,--verbose", cli::FlagType::Boolean,
                      "Enable verbose output");

    return executor.run(argc, argv);
}
```

## Key Features

- **Arbitrary nesting depth**: Commands can be nested to any depth
- **Automatic help generation**: Help text shows available subcommands at each level
- **Command-specific flags**: Each nested command can have its own flags
- **Global flags**: Flags that work across all commands
- **Positional arguments**: Still supported with nested commands
- **Backward compatible**: Existing single-level commands still work

## API

### Adding Nested Commands

```cpp
// Dot-separated path notation
executor.add_nested_command("parent.child.grandchild", 
                           "Description",
                           callback);
```

### Adding Flags to Nested Commands

```cpp
executor.add_nested_command_flag("parent.child.grandchild",
                                "-f,--flag",
                                cli::FlagType::Boolean,
                                "Flag description");
```

### Accessing Command Path in Callback

```cpp
[](const cli::ParseResult& result) {
    // Access the full command path
    for (const auto& cmd : result.command_path) {
        std::printf("Command level: %s\n", cmd.c_str());
    }
    return 0;
}
```


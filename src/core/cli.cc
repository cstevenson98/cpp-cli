#include "cli.hpp"

#include <algorithm>
#include <sstream>

namespace cli {

namespace {
// Helper for C++17 compatibility (starts_with is C++20)
bool starts_with(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}
} // namespace

// ParseResult implementation
bool ParseResult::get_bool(const std::string& flag_name) const {
    auto it = flags.find(flag_name);
    if (it == flags.end()) return false;
    if (auto* val = std::get_if<bool>(&it->second)) {
        return *val;
    }
    return false;
}

std::vector<std::string> ParseResult::get_args(const std::string& flag_name) const {
    auto it = flags.find(flag_name);
    if (it == flags.end()) return {};
    if (auto* val = std::get_if<std::vector<std::string>>(&it->second)) {
        return *val;
    }
    return {};
}

bool ParseResult::has_flag(const std::string& flag_name) const {
    return flags.find(flag_name) != flags.end();
}

// CliExecutor implementation

CliExecutor::CliExecutor(std::string program_name, std::string description)
    : program_name_(std::move(program_name))
    , description_(std::move(description)) {
    // Add built-in help flag
    add_flag("-h,--help", FlagType::Boolean, "Show help message");
}

void CliExecutor::set_usage(const std::string& usage) {
    usage_ = usage;
}

void CliExecutor::set_handler(CommandCallback callback) {
    default_handler_ = std::move(callback);
}

std::pair<std::string, std::string> CliExecutor::parse_flag_names(const std::string& names) {
    std::string short_name, long_name;
    
    size_t comma_pos = names.find(',');
    if (comma_pos != std::string::npos) {
        std::string first = names.substr(0, comma_pos);
        std::string second = names.substr(comma_pos + 1);
        
        // Trim whitespace
        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t"));
            s.erase(s.find_last_not_of(" \t") + 1);
        };
        trim(first);
        trim(second);
        
        if (starts_with(first, "--")) {
            long_name = first;
            short_name = second;
        } else {
            short_name = first;
            long_name = second;
        }
    } else {
        if (starts_with(names, "--")) {
            long_name = names;
        } else {
            short_name = names;
        }
    }
    
    return {short_name, long_name};
}

void CliExecutor::add_flag(const std::string& names, FlagType type,
                           const std::string& description, bool required) {
    auto [short_name, long_name] = parse_flag_names(names);
    global_flags_.push_back({short_name, long_name, type, description, required});
}

void CliExecutor::add_command(const std::string& name, const std::string& description,
                              CommandCallback callback) {
    commands_[name] = {name, description, std::move(callback), {}, {}};
}

std::vector<std::string> CliExecutor::split_path(const std::string& path, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    return parts;
}

CommandDef* CliExecutor::find_command(const std::vector<std::string>& path) {
    if (path.empty()) return nullptr;
    
    auto it = commands_.find(path[0]);
    if (it == commands_.end()) return nullptr;
    
    CommandDef* current = &it->second;
    for (size_t i = 1; i < path.size(); ++i) {
        auto sub_it = current->subcommands.find(path[i]);
        if (sub_it == current->subcommands.end()) return nullptr;
        current = &sub_it->second;
    }
    return current;
}

const CommandDef* CliExecutor::find_command(const std::vector<std::string>& path) const {
    if (path.empty()) return nullptr;
    
    auto it = commands_.find(path[0]);
    if (it == commands_.end()) return nullptr;
    
    const CommandDef* current = &it->second;
    for (size_t i = 1; i < path.size(); ++i) {
        auto sub_it = current->subcommands.find(path[i]);
        if (sub_it == current->subcommands.end()) return nullptr;
        current = &sub_it->second;
    }
    return current;
}

void CliExecutor::add_nested_command(const std::string& command_path,
                                     const std::string& description,
                                     CommandCallback callback) {
    auto parts = split_path(command_path, '.');
    if (parts.empty()) return;
    
    // Ensure first level command exists
    if (commands_.find(parts[0]) == commands_.end()) {
        commands_[parts[0]] = {parts[0], "", nullptr, {}, {}};
    }
    
    CommandDef* current = &commands_[parts[0]];
    
    // Navigate/create nested structure
    for (size_t i = 1; i < parts.size(); ++i) {
        if (current->subcommands.find(parts[i]) == current->subcommands.end()) {
            current->subcommands[parts[i]] = {parts[i], "", nullptr, {}, {}};
        }
        current = &current->subcommands[parts[i]];
    }
    
    // Set the final command's properties
    current->description = description;
    current->callback = std::move(callback);
}

void CliExecutor::add_command_flag(const std::string& command_name, const std::string& names,
                                   FlagType type, const std::string& description,
                                   bool required) {
    auto it = commands_.find(command_name);
    if (it != commands_.end()) {
        auto [short_name, long_name] = parse_flag_names(names);
        it->second.flags.push_back({short_name, long_name, type, description, required});
    }
}

void CliExecutor::add_nested_command_flag(const std::string& command_path,
                                          const std::string& names, FlagType type,
                                          const std::string& description,
                                          bool required) {
    auto parts = split_path(command_path, '.');
    CommandDef* cmd = find_command(parts);
    if (cmd) {
        auto [short_name, long_name] = parse_flag_names(names);
        cmd->flags.push_back({short_name, long_name, type, description, required});
    }
}

const FlagDef* CliExecutor::find_flag(const std::string& name,
                                      const std::vector<FlagDef>& command_flags) const {
    // Check command-specific flags first
    for (const auto& flag : command_flags) {
        if (flag.short_name == name || flag.long_name == name) {
            return &flag;
        }
    }
    // Check global flags
    for (const auto& flag : global_flags_) {
        if (flag.short_name == name || flag.long_name == name) {
            return &flag;
        }
    }
    return nullptr;
}

std::string CliExecutor::canonical_name(const FlagDef& flag) {
    return flag.long_name.empty() ? flag.short_name : flag.long_name;
}

ParseResult CliExecutor::parse(int argc, char* argv[]) const {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

ParseResult CliExecutor::parse(const std::vector<std::string>& args) const {
    ParseResult result;
    result.success = true;
    
    // Command-less mode: if we have a default handler and no commands
    bool commandless_mode = default_handler_ && commands_.empty();
    
    if (args.empty()) {
        if (!commandless_mode) {
            result.success = false;
            result.error_message = "No command specified";
        }
        return result;
    }
    
    size_t i = 0;
    std::vector<FlagDef> command_flags;
    const CommandDef* current_command = nullptr;
    
    // In command-less mode, all non-flag args are positional
    if (!commandless_mode && !starts_with(args[0], "-")) {
        // Parse command hierarchy
        while (i < args.size() && !starts_with(args[i], "-")) {
            std::string potential_command = args[i];
            
            // Check if this is a command at current level
            if (result.command_path.empty()) {
                // Top-level command
                auto cmd_it = commands_.find(potential_command);
                if (cmd_it != commands_.end()) {
                    result.command_path.push_back(potential_command);
                    current_command = &cmd_it->second;
                    command_flags = cmd_it->second.flags;
                    ++i;
                    
                    // Check for subcommands
                    while (i < args.size() && !starts_with(args[i], "-")) {
                        auto sub_it = current_command->subcommands.find(args[i]);
                        if (sub_it != current_command->subcommands.end()) {
                            result.command_path.push_back(args[i]);
                            current_command = &sub_it->second;
                            // Accumulate flags from each level
                            command_flags.insert(command_flags.end(),
                                               sub_it->second.flags.begin(),
                                               sub_it->second.flags.end());
                            ++i;
                        } else {
                            // Not a subcommand, must be a positional arg or flag
                            break;
                        }
                    }
                } else if (!commands_.empty()) {
                    result.success = false;
                    result.error_message = "Unknown command: " + potential_command;
                    return result;
                } else {
                    break;
                }
            }
            
            break;
        }
        
        // Set the legacy single command field to the full path
        if (!result.command_path.empty()) {
            result.command = result.command_path.back();
        }
    }
    
    // Parse remaining arguments
    while (i < args.size()) {
        const std::string& arg = args[i];
        
        if (starts_with(arg, "--")) {
            // Long form flag
            std::string flag_name;
            std::string flag_value;
            
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
                // --flag=value or --flag=val1,val2
                flag_name = arg.substr(0, eq_pos);
                flag_value = arg.substr(eq_pos + 1);
            } else {
                flag_name = arg;
            }
            
            const FlagDef* flag_def = find_flag(flag_name, command_flags);
            if (!flag_def) {
                result.success = false;
                result.error_message = "Unknown flag: " + flag_name;
                return result;
            }
            
            std::string canon = canonical_name(*flag_def);
            
            if (flag_def->type == FlagType::Boolean) {
                result.flags[canon] = true;
            } else {
                // MultiArg
                std::vector<std::string> values;
                
                if (!flag_value.empty()) {
                    // Split comma-separated values
                    std::stringstream ss(flag_value);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        if (!item.empty()) {
                            values.push_back(item);
                        }
                    }
                } else {
                    // Collect following non-flag arguments
                    ++i;
                    while (i < args.size() && !starts_with(args[i], "-")) {
                        values.push_back(args[i]);
                        ++i;
                    }
                    --i; // Adjust for loop increment
                }
                
                result.flags[canon] = values;
            }
        } else if (starts_with(arg, "-") && arg.length() > 1) {
            // Short form flag
            const FlagDef* flag_def = find_flag(arg, command_flags);
            if (!flag_def) {
                result.success = false;
                result.error_message = "Unknown flag: " + arg;
                return result;
            }
            
            std::string canon = canonical_name(*flag_def);
            
            if (flag_def->type == FlagType::Boolean) {
                result.flags[canon] = true;
            } else {
                // MultiArg - collect following non-flag arguments
                std::vector<std::string> values;
                ++i;
                while (i < args.size() && !starts_with(args[i], "-")) {
                    values.push_back(args[i]);
                    ++i;
                }
                --i; // Adjust for loop increment
                
                result.flags[canon] = values;
            }
        } else {
            // Positional argument
            result.positional_args.push_back(arg);
        }
        
        ++i;
    }
    
    // Check required flags
    auto check_required = [&](const std::vector<FlagDef>& flags) {
        for (const auto& flag : flags) {
            if (flag.required) {
                std::string canon = canonical_name(flag);
                if (!result.has_flag(canon)) {
                    result.success = false;
                    result.error_message = "Missing required flag: " + canon;
                    return false;
                }
            }
        }
        return true;
    };
    
    if (!check_required(global_flags_)) return result;
    if (!check_required(command_flags)) return result;
    
    return result;
}

int CliExecutor::execute(const ParseResult& result) const {
    if (!result.success) {
        return -1;
    }
    
    // Check for help flag
    if (result.get_bool("--help")) {
        if (result.command_path.empty()) {
            std::printf("%s", help().c_str());
        } else {
            std::printf("%s", help(result.command_path).c_str());
        }
        return 0;
    }
    
    // Command-less mode: use default handler
    if (result.command_path.empty() && default_handler_) {
        return default_handler_(result);
    }
    
    if (result.command_path.empty()) {
        return -1;
    }
    
    // Find the command using the path
    const CommandDef* cmd = find_command(result.command_path);
    if (!cmd || !cmd->callback) {
        // Command exists but has no callback - show help for this level
        std::printf("%s", help(result.command_path).c_str());
        return 0;
    }
    
    return cmd->callback(result);
}

int CliExecutor::run(int argc, char* argv[]) const {
    auto result = parse(argc, argv);
    if (!result.success) {
        std::fprintf(stderr, "Error: %s\n", result.error_message.c_str());
        std::fprintf(stderr, "Use --help for usage information.\n");
        return -1;
    }
    return execute(result);
}

std::string CliExecutor::help() const {
    std::ostringstream ss;
    ss << program_name_;
    if (!description_.empty()) {
        ss << " - " << description_;
    }
    ss << "\n\n";
    
    // Use custom usage if set, otherwise generate based on mode
    if (!usage_.empty()) {
        ss << "Usage: " << program_name_ << " " << usage_ << "\n\n";
    } else if (commands_.empty()) {
        ss << "Usage: " << program_name_ << " [options] [args...]\n\n";
    } else {
        ss << "Usage: " << program_name_ << " <command> [options]\n\n";
    }
    
    if (!commands_.empty()) {
        ss << "Commands:\n";
        for (const auto& [name, cmd] : commands_) {
            ss << "  " << name;
            if (!cmd.description.empty()) {
                ss << "\t" << cmd.description;
            }
            ss << "\n";
        }
        ss << "\n";
    }
    
    if (!global_flags_.empty()) {
        ss << "Options:\n";
        for (const auto& flag : global_flags_) {
            ss << "  ";
            if (!flag.short_name.empty()) {
                ss << flag.short_name;
                if (!flag.long_name.empty()) {
                    ss << ", ";
                }
            }
            if (!flag.long_name.empty()) {
                ss << flag.long_name;
            }
            if (flag.type == FlagType::MultiArg) {
                ss << " <args>";
            }
            if (!flag.description.empty()) {
                ss << "\t" << flag.description;
            }
            if (flag.required) {
                ss << " (required)";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string CliExecutor::help(const std::string& command_name) const {
    return help(std::vector<std::string>{command_name});
}

std::string CliExecutor::help(const std::vector<std::string>& command_path) const {
    const CommandDef* cmd = find_command(command_path);
    if (!cmd) {
        std::string path_str;
        for (size_t i = 0; i < command_path.size(); ++i) {
            if (i > 0) path_str += " ";
            path_str += command_path[i];
        }
        return "Unknown command: " + path_str + "\n";
    }
    
    std::ostringstream ss;
    
    // Build command string
    std::string cmd_str = program_name_;
    for (const auto& part : command_path) {
        cmd_str += " " + part;
    }
    
    ss << cmd_str;
    if (!cmd->description.empty()) {
        ss << " - " << cmd->description;
    }
    ss << "\n\n";
    
    ss << "Usage: " << cmd_str;
    if (!cmd->subcommands.empty()) {
        ss << " <subcommand>";
    }
    ss << " [options]\n\n";
    
    // Show subcommands if any
    if (!cmd->subcommands.empty()) {
        ss << "Subcommands:\n";
        for (const auto& [name, subcmd] : cmd->subcommands) {
            ss << "  " << name;
            if (!subcmd.description.empty()) {
                ss << "\t" << subcmd.description;
            }
            ss << "\n";
        }
        ss << "\n";
    }
    
    if (!cmd->flags.empty()) {
        ss << "Command Options:\n";
        for (const auto& flag : cmd->flags) {
            ss << "  ";
            if (!flag.short_name.empty()) {
                ss << flag.short_name;
                if (!flag.long_name.empty()) {
                    ss << ", ";
                }
            }
            if (!flag.long_name.empty()) {
                ss << flag.long_name;
            }
            if (flag.type == FlagType::MultiArg) {
                ss << " <args>";
            }
            if (!flag.description.empty()) {
                ss << "\t" << flag.description;
            }
            if (flag.required) {
                ss << " (required)";
            }
            ss << "\n";
        }
        ss << "\n";
    }
    
    if (!global_flags_.empty()) {
        ss << "Global Options:\n";
        for (const auto& flag : global_flags_) {
            ss << "  ";
            if (!flag.short_name.empty()) {
                ss << flag.short_name;
                if (!flag.long_name.empty()) {
                    ss << ", ";
                }
            }
            if (!flag.long_name.empty()) {
                ss << flag.long_name;
            }
            if (flag.type == FlagType::MultiArg) {
                ss << " <args>";
            }
            if (!flag.description.empty()) {
                ss << "\t" << flag.description;
            }
            if (flag.required) {
                ss << " (required)";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

} // namespace cli


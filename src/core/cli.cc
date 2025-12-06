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
    commands_[name] = {name, description, std::move(callback), {}};
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
    
    // In command-less mode, all non-flag args are positional
    if (!commandless_mode && !starts_with(args[0], "-")) {
        result.command = args[0];
        auto cmd_it = commands_.find(result.command);
        if (cmd_it != commands_.end()) {
            command_flags = cmd_it->second.flags;
        } else if (!commands_.empty()) {
            result.success = false;
            result.error_message = "Unknown command: " + result.command;
            return result;
        }
        ++i;
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
        if (result.command.empty()) {
            std::printf("%s", help().c_str());
        } else {
            std::printf("%s", help(result.command).c_str());
        }
        return 0;
    }
    
    // Command-less mode: use default handler
    if (result.command.empty() && default_handler_) {
        return default_handler_(result);
    }
    
    if (result.command.empty()) {
        return -1;
    }
    
    auto it = commands_.find(result.command);
    if (it == commands_.end()) {
        return -1;
    }
    
    return it->second.callback(result);
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
    auto it = commands_.find(command_name);
    if (it == commands_.end()) {
        return "Unknown command: " + command_name + "\n";
    }
    
    const auto& cmd = it->second;
    std::ostringstream ss;
    
    ss << program_name_ << " " << command_name;
    if (!cmd.description.empty()) {
        ss << " - " << cmd.description;
    }
    ss << "\n\n";
    
    ss << "Usage: " << program_name_ << " " << command_name << " [options]\n\n";
    
    if (!cmd.flags.empty()) {
        ss << "Command Options:\n";
        for (const auto& flag : cmd.flags) {
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


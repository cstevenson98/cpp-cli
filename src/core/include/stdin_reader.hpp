#pragma once

#include <deque>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

namespace cli {

/// Utility class for reading from stdin
class StdinReader {
public:
    /// Check if stdin has piped data (not a TTY)
    static bool has_piped_input() {
        return !isatty(fileno(stdin));
    }
    
    /// Read all content from stdin as a single string
    static std::string read_all() {
        std::string content;
        std::string line;
        while (std::getline(std::cin, line)) {
            content += line + '\n';
        }
        // Remove trailing newline if present
        if (!content.empty() && content.back() == '\n') {
            content.pop_back();
        }
        return content;
    }
    
    /// Read all lines from stdin into a vector
    static std::vector<std::string> read_lines() {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(std::cin, line)) {
            lines.push_back(line);
        }
        return lines;
    }
    
    /// Read last N lines from stdin (memory efficient for large inputs)
    static std::vector<std::string> read_last_n_lines(std::size_t n) {
        if (n == 0) return {};
        
        std::deque<std::string> buffer;
        std::string line;
        
        while (std::getline(std::cin, line)) {
            buffer.push_back(std::move(line));
            if (buffer.size() > n) {
                buffer.pop_front();
            }
        }
        
        return std::vector<std::string>(buffer.begin(), buffer.end());
    }
    
    /// Read first N lines from stdin
    static std::vector<std::string> read_first_n_lines(std::size_t n) {
        std::vector<std::string> lines;
        lines.reserve(n);
        std::string line;
        
        while (lines.size() < n && std::getline(std::cin, line)) {
            lines.push_back(std::move(line));
        }
        
        return lines;
    }
    
    /// Read stdin or file content
    /// If filename is provided and not "-", reads from file
    /// Otherwise reads from stdin
    static std::optional<std::vector<std::string>> read_lines_from(
            const std::string& source) {
        if (source.empty() || source == "-") {
            return read_lines();
        }
        
        // Read from file
        std::ifstream file(source);
        if (!file) {
            return std::nullopt;
        }
        
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        return lines;
    }
};

} // namespace cli


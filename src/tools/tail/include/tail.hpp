#pragma once

#include <deque>
#include <fstream>
#include <string>
#include <vector>

namespace tail {

/// Result of a tail operation
struct TailResult {
    bool success = false;
    std::string error_message;
    std::vector<std::string> lines;
};

/// Get the last N lines from a vector of lines
inline std::vector<std::string> last_n_lines(
        const std::vector<std::string>& all_lines, std::size_t n) {
    if (n >= all_lines.size()) {
        return all_lines;
    }
    return std::vector<std::string>(
        all_lines.end() - static_cast<std::ptrdiff_t>(n),
        all_lines.end()
    );
}

/// Read last N lines from a file
inline TailResult tail_file(const std::string& filename, std::size_t n) {
    TailResult result;
    
    std::ifstream file(filename);
    if (!file) {
        result.error_message = "Cannot open file: " + filename;
        return result;
    }
    
    // Use a circular buffer to keep only last N lines
    std::deque<std::string> buffer;
    std::string line;
    
    while (std::getline(file, line)) {
        buffer.push_back(std::move(line));
        if (buffer.size() > n) {
            buffer.pop_front();
        }
    }
    
    result.success = true;
    result.lines = std::vector<std::string>(buffer.begin(), buffer.end());
    return result;
}

/// Read last N lines from stdin (via provided lines vector)
inline TailResult tail_lines(const std::vector<std::string>& input_lines, 
                             std::size_t n) {
    TailResult result;
    result.success = true;
    result.lines = last_n_lines(input_lines, n);
    return result;
}

} // namespace tail


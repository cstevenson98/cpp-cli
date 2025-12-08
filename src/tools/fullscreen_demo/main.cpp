#include <chrono>
#include <cstdio>
#include <string>
#include <vector>

#include "cli.hpp"
#include "fullscreen_terminal.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("fullscreen_demo",
                            "Demonstrates fullscreen terminal mode");

  executor.add_command(
      "pattern", "Display alternating patterns in fullscreen mode",
      [](const cli::ParseResult & /* result */) {
        cli::FullscreenTerminal terminal;

        // Track which pattern to show
        bool show_dashes = true;
        auto start_time = std::chrono::steady_clock::now();
        const int cycle_duration_ms = 2000; // 2 seconds

        terminal.enter();

        // Main loop - returns false to exit
        auto update_callback = [&]() -> bool {
          // Calculate elapsed time
          auto now = std::chrono::steady_clock::now();
          auto elapsed_ms =
              std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                    start_time)
                  .count();

          // Switch patterns every 2 seconds
          if (elapsed_ms >= cycle_duration_ms) {
            show_dashes = !show_dashes;
            start_time = now;
          }

          // Get terminal size
          auto size = terminal.get_size();

          // Create the pattern
          std::vector<std::string> content;
          std::string pattern = show_dashes ? "----" : "++++";

          for (int row = 0; row < size.rows; ++row) {
            std::string line;
            // Fill the line with the pattern
            while (line.length() < static_cast<size_t>(size.cols)) {
              line += pattern;
            }
            // Truncate to exact width
            if (line.length() > static_cast<size_t>(size.cols)) {
              line = line.substr(0, size.cols);
            }
            content.push_back(line);
          }

          // Update the terminal content
          terminal.set_content(content);

          return true; // Continue loop
        };

        // Run the loop with 100ms update interval
        terminal.run_loop(update_callback, 100);

        std::printf("Fullscreen demo exited. (Press 'q' or Ctrl-C to exit)\n");
        return 0;
      });

  executor.add_command(
      "blocks", "Display alternating block patterns",
      [](const cli::ParseResult & /* result */) {
        cli::FullscreenTerminal terminal;

        bool show_solid = true;
        auto start_time = std::chrono::steady_clock::now();
        const int cycle_duration_ms = 2000;

        terminal.enter();

        auto update_callback = [&]() -> bool {
          auto now = std::chrono::steady_clock::now();
          auto elapsed_ms =
              std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                    start_time)
                  .count();

          if (elapsed_ms >= cycle_duration_ms) {
            show_solid = !show_solid;
            start_time = now;
          }

          auto size = terminal.get_size();
          std::vector<std::string> content;

          // Create checkerboard or solid pattern
          for (int row = 0; row < size.rows; ++row) {
            std::string line;
            if (show_solid) {
              // Solid blocks
              line = std::string(size.cols, '#');
            } else {
              // Checkerboard
              for (int col = 0; col < size.cols; ++col) {
                if ((row + col) % 2 == 0) {
                  line += '#';
                } else {
                  line += ' ';
                }
              }
            }
            content.push_back(line);
          }

          terminal.set_content(content);
          return true;
        };

        terminal.run_loop(update_callback, 100);

        std::printf("Fullscreen demo exited. (Press 'q' or Ctrl-C to exit)\n");
        return 0;
      });

  executor.add_command(
      "counter", "Display a running counter with status info",
      [](const cli::ParseResult & /* result */) {
        cli::FullscreenTerminal terminal;

        int counter = 0;
        auto start_time = std::chrono::steady_clock::now();

        terminal.enter();

        auto update_callback = [&]() -> bool {
          auto now = std::chrono::steady_clock::now();
          auto elapsed_s =
              std::chrono::duration_cast<std::chrono::seconds>(now - start_time)
                  .count();

          auto size = terminal.get_size();
          std::vector<std::string> content;

          // Header
          content.push_back(std::string(size.cols, '='));
          content.push_back("  FULLSCREEN TERMINAL DEMO");
          content.push_back(std::string(size.cols, '='));
          content.push_back("");

          // Info
          char buf[256];
          std::snprintf(buf, sizeof(buf), "  Terminal size: %d rows x %d cols",
                        size.rows, size.cols);
          content.push_back(buf);

          std::snprintf(buf, sizeof(buf), "  Elapsed time: %ld seconds",
                        elapsed_s);
          content.push_back(buf);

          std::snprintf(buf, sizeof(buf), "  Counter: %d", counter++);
          content.push_back(buf);

          content.push_back("");
          content.push_back("  Press 'q' or Ctrl-C to exit");

          terminal.set_content(content);
          return true;
        };

        terminal.run_loop(update_callback, 100);

        std::printf("Fullscreen demo exited.\n");
        return 0;
      });

  executor.add_command(
      "pascal", "Display Pascal's triangle, growing one row at a time",
      [](const cli::ParseResult & /* result */) {
        cli::FullscreenTerminal terminal;

        int current_row = 0;
        auto start_time = std::chrono::steady_clock::now();
        const int row_add_interval_ms = 500; // Add a new row every 500ms

        // Function to compute Pascal's triangle row
        auto compute_pascal_row = [](int row_num) -> std::vector<long long> {
          std::vector<long long> result;
          result.push_back(1);
          for (int k = 1; k <= row_num; ++k) {
            long long val = result[k - 1] * (row_num - k + 1) / k;
            result.push_back(val);
          }
          return result;
        };

        // Function to format a row as a string
        auto format_row = [](const std::vector<long long> &row) -> std::string {
          std::string result;
          for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) {
              result += "  ";
            }
            result += std::to_string(row[i]);
          }
          return result;
        };

        terminal.enter();

        auto update_callback = [&]() -> bool {
          auto now = std::chrono::steady_clock::now();
          auto elapsed_ms =
              std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                    start_time)
                  .count();

          // Add a new row every interval
          int target_row = static_cast<int>(elapsed_ms / row_add_interval_ms);

          auto size = terminal.get_size();
          std::vector<std::string> content;

          // Header
          content.push_back("Pascal's Triangle");
          content.push_back("");

          // Calculate how many rows we can display
          int max_displayable_rows =
              size.rows - 5; // Reserve space for header/footer
          if (target_row > max_displayable_rows) {
            target_row = max_displayable_rows;
          }

          current_row = target_row;

          // Build the triangle
          std::vector<std::string> triangle_rows;
          for (int r = 0; r <= current_row; ++r) {
            auto row_data = compute_pascal_row(r);
            std::string row_str = format_row(row_data);
            triangle_rows.push_back(row_str);
          }

          // Center each row
          for (const auto &row_str : triangle_rows) {
            int padding = (size.cols - static_cast<int>(row_str.length())) / 2;
            if (padding < 0) {
              padding = 0;
            }
            std::string centered = std::string(padding, ' ') + row_str;
            content.push_back(centered);
          }

          // Footer
          content.push_back("");
          char buf[256];
          std::snprintf(buf, sizeof(buf), "Row: %d", current_row);
          content.push_back(buf);
          content.push_back("Press 'q' or Ctrl-C to exit");

          terminal.set_content(content);
          return true;
        };

        terminal.run_loop(update_callback, 100);

        std::printf("Pascal's triangle demo exited.\n");
        return 0;
      });

  return executor.run(argc, argv);
}

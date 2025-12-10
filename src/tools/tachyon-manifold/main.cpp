#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cli.hpp"
#include "fullscreen_terminal.hpp"
#include "stdin_reader.hpp"
#include "tachyon_manifold.hpp"

int main(int argc, char *argv[]) {
  cli::CliExecutor executor("tachyon-manifold",
                            "Interactive display of piped data");

  executor.add_command(
      "interactive", "Display piped input line by line in fullscreen mode",
      [](const cli::ParseResult & /* result */) {
        if (!cli::StdinReader::has_piped_input()) {
          std::cerr << "Error: No input provided. Pipe data to stdin.\n";
          std::cerr << "Example: cat file.txt | tachyon-manifold interactive\n";
          return 1;
        }

        // Read all lines from stdin
        auto lines = cli::StdinReader::read_lines();

        if (lines.empty()) {
          std::cerr << "Error: No lines to display.\n";
          return 1;
        }

        tachyons::TachyonManifold manifold(lines);

        cli::FullscreenTerminal terminal;

        const int tick_interval_ms = 500; // 500ms between updates

        terminal.enter();

        auto update_callback = [&]() -> bool {
          auto size = terminal.get_size();
          std::vector<std::string> content;

          // Header
          content.emplace_back(
              "=== Tachyon Manifold - Interactive Display ===");
          content.emplace_back();

          // Get the current state of the manifold
          auto manifold_lines = manifold.get_manifold_lines();

          // Calculate how many lines we can show
          int max_display_lines = size.rows - 5; // Reserve for header/footer

          // Determine which lines to display
          int start_line = 0;
          int end_line = static_cast<int>(manifold_lines.size()) - 1;

          // If we have more lines than can fit, show the most recent ones
          if (end_line - start_line + 1 > max_display_lines) {
            start_line = end_line - max_display_lines + 1;
          }

          // Add the lines
          for (int i = start_line; i <= end_line; ++i) {
            std::string line = manifold_lines[i];
            // Truncate lines that are too long
            if (line.length() > static_cast<size_t>(size.cols)) {
              line = line.substr(0, size.cols - 3) + "...";
            }
            content.push_back(line);
          }

          // Footer
          content.emplace_back();
          std::ostringstream footer;
          footer << "Row " << manifold.get_rows_completed() + 1 << " of "
                 << manifold.get_total_rows()
                 << " | Press 'q' or Ctrl-C to exit";
          content.emplace_back(footer.str());

          terminal.set_content(content);

          // Update the manifold for the next tick
          if (manifold.has_more_rows()) {
            manifold.update_manifold();
          } else {
            std::cout << manifold.get_beam_splits_count() << '\n';
            return false;
          }
          return true;
        };

        terminal.run_loop(update_callback, tick_interval_ms);

        std::cout << "Interactive display exited.\n";
        std::cout << "Processed " << manifold.get_total_rows()
                  << " rows total.\n";
        return 0;
      });

  executor.add_command(
      "solve", "Solve the tachyon manifold",
      [](const cli::ParseResult & /* result */) {
        if (!cli::StdinReader::has_piped_input()) {
          std::cerr << "Error: No input provided. Pipe data to stdin.\n";
          std::cerr << "Example: cat file.txt | tachyon-manifold solve\n";
          return 1;
        }
        auto lines = cli::StdinReader::read_lines();
        tachyons::TachyonManifold manifold(lines);
        auto result = manifold.solve();
        std::cout << result.total_timelines << '\n';
        std::cout << result.beam_splits_count << '\n';
        return 0;
      });
  return executor.run(argc, argv);
}

#include <cstdio>
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
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin.\n");
          std::fprintf(
              stderr, "Example: cat file.txt | tachyon-manifold interactive\n");
          return 1;
        }

        // Read all lines from stdin
        auto lines = cli::StdinReader::read_lines();

        if (lines.empty()) {
          std::fprintf(stderr, "Error: No lines to display.\n");
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
          content.push_back("=== Tachyon Manifold - Interactive Display ===");
          content.push_back("");

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
          content.push_back("");
          char buf[256];
          std::snprintf(
              buf, sizeof(buf), "Row %d of %d | Press 'q' or Ctrl-C to exit",
              manifold.get_rows_completed() + 1, manifold.get_total_rows());
          content.push_back(buf);

          terminal.set_content(content);

          // Update the manifold for the next tick
          if (manifold.has_more_rows()) {
            manifold.update_manifold();
          } else {
            std::printf("%d\n", manifold.get_beam_splits_count());
            return false;
          }
          return true;
        };

        terminal.run_loop(update_callback, tick_interval_ms);

        std::printf("Interactive display exited.\n");
        std::printf("Processed %d rows total.\n", manifold.get_total_rows());
        return 0;
      });

  executor.add_command(
      "solve", "Solve the tachyon manifold",
      [](const cli::ParseResult & /* result */) {
        if (!cli::StdinReader::has_piped_input()) {
          std::fprintf(stderr,
                       "Error: No input provided. Pipe data to stdin.\n");
          std::fprintf(stderr,
                       "Example: cat file.txt | tachyon-manifold solve\n");
          return 1;
        }
        auto lines = cli::StdinReader::read_lines();
        tachyons::TachyonManifold manifold(lines);
        int beam_splits_count = manifold.solve();
        std::printf("%d\n", beam_splits_count);
        return 0;
      });
  return executor.run(argc, argv);
}

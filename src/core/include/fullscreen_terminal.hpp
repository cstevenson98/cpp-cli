#pragma once

#include <csignal>
#include <functional>
#include <string>
#include <termios.h>
#include <vector>

namespace cli {

/// Fullscreen terminal window with frame buffering
/// Uses alternate screen buffer and only redraws changed content
class FullscreenTerminal {
public:
  /// Instruction types that can be sent to the terminal
  enum class Instruction {
    UpdateScreen, // Update the entire screen
    Quit,         // Exit the loop
  };

  /// Callback function type for handling instructions
  using InstructionHandler = std::function<bool()>;

  FullscreenTerminal();
  ~FullscreenTerminal();

  // Disable copy and move
  FullscreenTerminal(const FullscreenTerminal &) = delete;
  FullscreenTerminal &operator=(const FullscreenTerminal &) = delete;
  FullscreenTerminal(FullscreenTerminal &&) = delete;
  FullscreenTerminal &operator=(FullscreenTerminal &&) = delete;

  /// Enter fullscreen mode (alternate screen buffer)
  void enter();

  /// Exit fullscreen mode and restore terminal
  void exit();

  /// Get current terminal dimensions
  struct Size {
    int rows;
    int cols;
  };
  Size get_size() const;

  /// Set the content of the screen
  /// @param content Vector of lines to display (one per row)
  void set_content(const std::vector<std::string> &content);

  /// Clear the screen
  void clear();

  /// Force a complete redraw (bypass frame buffer optimization)
  void force_redraw();

  /// Flush the frame buffer to the terminal (only draw changes)
  void flush();

  /// Run the main loop - blocks until quit() is called or Ctrl-C
  /// @param update_callback Called periodically to update content
  /// @param interval_ms Milliseconds between updates (default 100ms)
  void run_loop(InstructionHandler update_callback, int interval_ms = 100);

  /// Signal to quit the main loop
  void quit();

  /// Check if currently in fullscreen mode
  bool is_fullscreen() const { return in_fullscreen_; }

  /// Check if a key has been pressed (non-blocking)
  /// @return Character code if key pressed, -1 if no key pressed
  int get_key();

private:
  bool in_fullscreen_;
  bool should_quit_;
  bool force_redraw_;
  Size cached_size_;
  std::vector<std::string> current_buffer_; // What's currently on screen
  std::vector<std::string> pending_buffer_; // What we want to display

  // Terminal state
  struct termios orig_termios_;
  bool termios_saved_;

  // Terminal control
  void hide_cursor();
  void show_cursor();
  void move_cursor(int row, int col);
  void save_cursor();
  void restore_cursor();
  void enter_alternate_screen();
  void exit_alternate_screen();
  void clear_screen();

  // Terminal mode control
  void enable_raw_mode();
  void disable_raw_mode();

  // Update cached terminal size
  void update_terminal_size();

  // Diff and update only changed lines
  void update_screen_diff();

  // Signal handling
  static void setup_signal_handlers();
  static void restore_signal_handlers();
  static volatile sig_atomic_t resize_flag_;
  static volatile sig_atomic_t interrupt_flag_;
  static void signal_handler(int signum);
};

} // namespace cli

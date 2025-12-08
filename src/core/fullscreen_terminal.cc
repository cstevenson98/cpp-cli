#include "fullscreen_terminal.hpp"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

namespace cli {

// Static members for signal handling
volatile sig_atomic_t FullscreenTerminal::resize_flag_ = 0;
volatile sig_atomic_t FullscreenTerminal::interrupt_flag_ = 0;

FullscreenTerminal::FullscreenTerminal()
    : in_fullscreen_(false), should_quit_(false), force_redraw_(false),
      cached_size_{0, 0}, termios_saved_(false) {
  update_terminal_size();
}

FullscreenTerminal::~FullscreenTerminal() {
  if (in_fullscreen_) {
    exit();
  }
}

void FullscreenTerminal::enter() {
  if (in_fullscreen_) {
    return;
  }

  setup_signal_handlers();
  enable_raw_mode();
  enter_alternate_screen();
  hide_cursor();
  clear_screen();
  update_terminal_size();

  // Initialize buffers
  current_buffer_.clear();
  pending_buffer_.clear();
  current_buffer_.resize(cached_size_.rows);
  pending_buffer_.resize(cached_size_.rows);

  in_fullscreen_ = true;
  should_quit_ = false;
}

void FullscreenTerminal::exit() {
  if (!in_fullscreen_) {
    return;
  }

  show_cursor();
  exit_alternate_screen();
  disable_raw_mode();
  restore_signal_handlers();

  in_fullscreen_ = false;
}

FullscreenTerminal::Size FullscreenTerminal::get_size() const {
  return cached_size_;
}

void FullscreenTerminal::update_terminal_size() {
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
    cached_size_.rows = w.ws_row;
    cached_size_.cols = w.ws_col;
  } else {
    // Fallback defaults
    cached_size_.rows = 24;
    cached_size_.cols = 80;
  }
}

void FullscreenTerminal::set_content(const std::vector<std::string> &content) {
  pending_buffer_ = content;

  // Ensure buffer is the right size
  if (pending_buffer_.size() < static_cast<size_t>(cached_size_.rows)) {
    pending_buffer_.resize(cached_size_.rows);
  } else if (pending_buffer_.size() > static_cast<size_t>(cached_size_.rows)) {
    pending_buffer_.resize(cached_size_.rows);
  }

  // Truncate or pad lines to fit width
  for (auto &line : pending_buffer_) {
    if (line.length() > static_cast<size_t>(cached_size_.cols)) {
      line = line.substr(0, cached_size_.cols);
    }
  }
}

void FullscreenTerminal::clear() {
  pending_buffer_.clear();
  pending_buffer_.resize(cached_size_.rows);
}

void FullscreenTerminal::force_redraw() { force_redraw_ = true; }

void FullscreenTerminal::flush() {
  if (!in_fullscreen_) {
    return;
  }

  // Check if we need to handle resize
  if (resize_flag_) {
    resize_flag_ = 0;
    update_terminal_size();
    current_buffer_.resize(cached_size_.rows);
    pending_buffer_.resize(cached_size_.rows);
    force_redraw_ = true;
  }

  if (force_redraw_) {
    // Full redraw
    clear_screen();
    current_buffer_.clear();
    current_buffer_.resize(cached_size_.rows);
    force_redraw_ = false;
  }

  update_screen_diff();
}

void FullscreenTerminal::update_screen_diff() {
  // Only update lines that have changed
  for (int row = 0; row < cached_size_.rows; ++row) {
    if (row >= static_cast<int>(pending_buffer_.size())) {
      break;
    }

    if (row >= static_cast<int>(current_buffer_.size()) ||
        current_buffer_[row] != pending_buffer_[row]) {
      // Line changed - update it
      move_cursor(row, 0);

      // Clear line and write new content
      std::printf("\033[K%s", pending_buffer_[row].c_str());
      std::fflush(stdout);

      if (row < static_cast<int>(current_buffer_.size())) {
        current_buffer_[row] = pending_buffer_[row];
      }
    }
  }
}

void FullscreenTerminal::run_loop(InstructionHandler update_callback,
                                  int interval_ms) {
  if (!in_fullscreen_) {
    enter();
  }

  should_quit_ = false;
  interrupt_flag_ = 0;

  while (!should_quit_ && !interrupt_flag_) {
    // Check for key presses (non-blocking)
    int key = get_key();
    if (key == 3 || key == 'q' || key == 'Q') {
      // Ctrl-C (ASCII 3) or 'q' key pressed - exit
      break;
    }

    // Call the update callback
    if (update_callback) {
      if (!update_callback()) {
        break; // Callback returned false, exit loop
      }
    }

    // Flush updates to screen
    flush();

    // Sleep for the specified interval
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
  }

  exit();
}

void FullscreenTerminal::quit() { should_quit_ = true; }

int FullscreenTerminal::get_key() {
  unsigned char ch;
  ssize_t n = read(STDIN_FILENO, &ch, 1);
  if (n == 1) {
    return ch;
  }
  return -1; // No key pressed
}

// Terminal control functions
void FullscreenTerminal::hide_cursor() {
  std::printf("\033[?25l");
  std::fflush(stdout);
}

void FullscreenTerminal::show_cursor() {
  std::printf("\033[?25h");
  std::fflush(stdout);
}

void FullscreenTerminal::move_cursor(int row, int col) {
  // ANSI escape: row and col are 1-based
  std::printf("\033[%d;%dH", row + 1, col + 1);
}

void FullscreenTerminal::save_cursor() {
  std::printf("\033[s");
  std::fflush(stdout);
}

void FullscreenTerminal::restore_cursor() {
  std::printf("\033[u");
  std::fflush(stdout);
}

void FullscreenTerminal::enter_alternate_screen() {
  // Use alternate screen buffer (like vim, less, top)
  std::printf("\033[?1049h");
  std::fflush(stdout);
}

void FullscreenTerminal::exit_alternate_screen() {
  // Exit alternate screen buffer
  std::printf("\033[?1049l");
  std::fflush(stdout);
}

void FullscreenTerminal::clear_screen() {
  std::printf("\033[2J\033[H");
  std::fflush(stdout);
}

void FullscreenTerminal::enable_raw_mode() {
  // Save original terminal settings
  if (tcgetattr(STDIN_FILENO, &orig_termios_) == -1) {
    return;
  }
  termios_saved_ = true;

  struct termios raw = orig_termios_;

  // Disable echo and canonical mode
  // ECHO: Don't echo input characters
  // ICANON: Disable canonical mode (line-by-line input)
  // ISIG: Disable signal generation (Ctrl-C, Ctrl-Z)
  // IEXTEN: Disable extended input processing
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

  // Disable special input processing
  // IXON: Disable Ctrl-S and Ctrl-Q
  // ICRNL: Don't translate CR to NL
  // BRKINT: Don't send SIGINT on break
  // INPCK: Disable parity checking
  // ISTRIP: Don't strip 8th bit
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

  // Disable output processing
  // OPOST: Disable output processing
  raw.c_oflag &= ~(OPOST);

  // Set character size to 8 bits
  raw.c_cflag |= (CS8);

  // Set read timeout (deciseconds)
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void FullscreenTerminal::disable_raw_mode() {
  if (termios_saved_) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_);
    termios_saved_ = false;
  }
}

// Signal handling
void FullscreenTerminal::signal_handler(int signum) {
  if (signum == SIGWINCH) {
    resize_flag_ = 1;
  } else if (signum == SIGINT) {
    interrupt_flag_ = 1;
  }
}

void FullscreenTerminal::setup_signal_handlers() {
  struct sigaction sa;
  std::memset(&sa, 0, sizeof(sa));
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGWINCH, &sa, nullptr); // Terminal resize
  sigaction(SIGINT, &sa, nullptr);   // Ctrl-C
}

void FullscreenTerminal::restore_signal_handlers() {
  signal(SIGWINCH, SIG_DFL);
  signal(SIGINT, SIG_DFL);
}

} // namespace cli

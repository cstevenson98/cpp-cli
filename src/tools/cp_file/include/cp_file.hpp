#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace cp_file {

/// Result of a copy operation
struct CopyResult {
  bool success = false;
  std::string error_message;
  std::size_t bytes_copied = 0;
};

/// Copy a file from source to destination
/// @param source Source file path
/// @param dest Destination file path
/// @param overwrite If true, overwrite existing destination file
/// @return CopyResult with status and bytes copied
inline CopyResult copy_file(const std::string& source, const std::string& dest,
                            bool overwrite = false) {
  CopyResult result;

  namespace fs = std::filesystem;

  // Check source exists
  if (!fs::exists(source)) {
    result.error_message = "Source file does not exist: " + source;
    return result;
  }

  // Check source is a regular file
  if (!fs::is_regular_file(source)) {
    result.error_message = "Source is not a regular file: " + source;
    return result;
  }

  // Check destination doesn't exist (unless overwrite)
  if (fs::exists(dest) && !overwrite) {
    result.error_message = "Destination already exists: " + dest;
    return result;
  }

  // Create destination directory if needed
  fs::path dest_path(dest);
  if (dest_path.has_parent_path()) {
    std::error_code ec;
    fs::create_directories(dest_path.parent_path(), ec);
    if (ec) {
      result.error_message =
          "Failed to create destination directory: " + ec.message();
      return result;
    }
  }

  // Perform copy
  try {
    auto options = fs::copy_options::none;
    if (overwrite) {
      options = fs::copy_options::overwrite_existing;
    }

    fs::copy_file(source, dest, options);
    result.success = true;
    result.bytes_copied = fs::file_size(dest);
  } catch (const fs::filesystem_error& e) {
    result.error_message = e.what();
  }

  return result;
}

}  // namespace cp_file

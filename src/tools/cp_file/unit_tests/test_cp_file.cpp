#include "cp_file.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>
#include <thread>

namespace cp_file {
namespace {

// Generate unique ID for test directories
std::string generate_unique_id() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
    std::random_device rd;
    return std::to_string(ns) + "_" + std::to_string(rd());
}

class CpFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a unique temp directory for each test
        test_dir_ = std::filesystem::temp_directory_path() / 
                    ("cp_file_test_" + generate_unique_id());
        std::filesystem::create_directories(test_dir_);
    }
    
    void TearDown() override {
        // Clean up temp directory
        std::error_code ec;
        std::filesystem::remove_all(test_dir_, ec);
    }
    
    // Helper to create a test file with content
    void create_test_file(const std::filesystem::path& path, const std::string& content) {
        std::filesystem::create_directories(path.parent_path());
        std::ofstream file(path);
        file << content;
    }
    
    // Helper to read file content
    std::string read_file_content(const std::filesystem::path& path) {
        std::ifstream file(path);
        return std::string(std::istreambuf_iterator<char>(file),
                          std::istreambuf_iterator<char>());
    }
    
    std::filesystem::path test_dir_;
};

TEST_F(CpFileTest, CopyFile_Success) {
    auto source = test_dir_ / "source.txt";
    auto dest = test_dir_ / "dest.txt";
    
    create_test_file(source, "Hello, World!");
    
    auto result = copy_file(source.string(), dest.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.error_message.empty());
    EXPECT_EQ(result.bytes_copied, 13u);
    EXPECT_TRUE(std::filesystem::exists(dest));
    EXPECT_EQ(read_file_content(dest), "Hello, World!");
}

TEST_F(CpFileTest, CopyFile_SourceNotExists) {
    auto source = test_dir_ / "nonexistent.txt";
    auto dest = test_dir_ / "dest.txt";
    
    auto result = copy_file(source.string(), dest.string());
    
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error_message.find("does not exist") != std::string::npos);
}

TEST_F(CpFileTest, CopyFile_DestinationExists_NoOverwrite) {
    auto source = test_dir_ / "source.txt";
    auto dest = test_dir_ / "dest.txt";
    
    create_test_file(source, "Source content");
    create_test_file(dest, "Existing content");
    
    auto result = copy_file(source.string(), dest.string(), false);
    
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error_message.find("already exists") != std::string::npos);
    EXPECT_EQ(read_file_content(dest), "Existing content");
}

TEST_F(CpFileTest, CopyFile_DestinationExists_WithOverwrite) {
    auto source = test_dir_ / "source.txt";
    auto dest = test_dir_ / "dest.txt";
    
    create_test_file(source, "New content");
    create_test_file(dest, "Old content");
    
    auto result = copy_file(source.string(), dest.string(), true);
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(read_file_content(dest), "New content");
}

TEST_F(CpFileTest, CopyFile_CreateDestinationDirectory) {
    auto source = test_dir_ / "source.txt";
    auto dest = test_dir_ / "subdir" / "nested" / "dest.txt";
    
    create_test_file(source, "Test content");
    
    auto result = copy_file(source.string(), dest.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(std::filesystem::exists(dest));
    EXPECT_EQ(read_file_content(dest), "Test content");
}

TEST_F(CpFileTest, CopyFile_SourceIsDirectory) {
    auto source = test_dir_ / "source_dir";
    auto dest = test_dir_ / "dest.txt";
    
    std::filesystem::create_directories(source);
    
    auto result = copy_file(source.string(), dest.string());
    
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error_message.find("not a regular file") != std::string::npos);
}

TEST_F(CpFileTest, CopyFile_EmptyFile) {
    auto source = test_dir_ / "empty.txt";
    auto dest = test_dir_ / "dest.txt";
    
    create_test_file(source, "");
    
    auto result = copy_file(source.string(), dest.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.bytes_copied, 0u);
    EXPECT_TRUE(std::filesystem::exists(dest));
}

TEST_F(CpFileTest, CopyFile_LargeFile) {
    auto source = test_dir_ / "large.txt";
    auto dest = test_dir_ / "dest.txt";
    
    // Create a 1MB file
    std::string content(1024 * 1024, 'X');
    create_test_file(source, content);
    
    auto result = copy_file(source.string(), dest.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.bytes_copied, 1024u * 1024u);
    EXPECT_EQ(read_file_content(dest), content);
}

} // namespace
} // namespace cp_file


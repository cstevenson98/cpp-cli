#include "tail.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <random>

namespace tail {
namespace {

// Generate unique ID for test files
std::string generate_unique_id() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
    std::random_device rd;
    return std::to_string(ns) + "_" + std::to_string(rd());
}

class TailTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir_ = std::filesystem::temp_directory_path() / 
                    ("tail_test_" + generate_unique_id());
        std::filesystem::create_directories(test_dir_);
    }
    
    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(test_dir_, ec);
    }
    
    void create_test_file(const std::filesystem::path& path, 
                          const std::vector<std::string>& lines) {
        std::ofstream file(path);
        for (const auto& line : lines) {
            file << line << '\n';
        }
    }
    
    std::filesystem::path test_dir_;
};

TEST_F(TailTest, LastNLines_Basic) {
    std::vector<std::string> lines = {"line1", "line2", "line3", "line4", "line5"};
    
    auto result = last_n_lines(lines, 3);
    
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "line3");
    EXPECT_EQ(result[1], "line4");
    EXPECT_EQ(result[2], "line5");
}

TEST_F(TailTest, LastNLines_MoreThanAvailable) {
    std::vector<std::string> lines = {"line1", "line2"};
    
    auto result = last_n_lines(lines, 10);
    
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
}

TEST_F(TailTest, LastNLines_Zero) {
    std::vector<std::string> lines = {"line1", "line2"};
    
    auto result = last_n_lines(lines, 0);
    
    EXPECT_TRUE(result.empty());
}

TEST_F(TailTest, LastNLines_EmptyInput) {
    std::vector<std::string> lines;
    
    auto result = last_n_lines(lines, 5);
    
    EXPECT_TRUE(result.empty());
}

TEST_F(TailTest, LastNLines_ExactCount) {
    std::vector<std::string> lines = {"line1", "line2", "line3"};
    
    auto result = last_n_lines(lines, 3);
    
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
    EXPECT_EQ(result[2], "line3");
}

TEST_F(TailTest, TailFile_Success) {
    auto file_path = test_dir_ / "test.txt";
    create_test_file(file_path, {"line1", "line2", "line3", "line4", "line5"});
    
    auto result = tail_file(file_path.string(), 3);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.error_message.empty());
    ASSERT_EQ(result.lines.size(), 3u);
    EXPECT_EQ(result.lines[0], "line3");
    EXPECT_EQ(result.lines[1], "line4");
    EXPECT_EQ(result.lines[2], "line5");
}

TEST_F(TailTest, TailFile_FileNotFound) {
    auto result = tail_file("/nonexistent/file.txt", 10);
    
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

TEST_F(TailTest, TailFile_EmptyFile) {
    auto file_path = test_dir_ / "empty.txt";
    create_test_file(file_path, {});
    
    auto result = tail_file(file_path.string(), 10);
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.lines.empty());
}

TEST_F(TailTest, TailFile_LargeFile) {
    auto file_path = test_dir_ / "large.txt";
    
    // Create file with 1000 lines
    std::vector<std::string> lines;
    for (int i = 0; i < 1000; ++i) {
        lines.push_back("line_" + std::to_string(i));
    }
    create_test_file(file_path, lines);
    
    auto result = tail_file(file_path.string(), 5);
    
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.lines.size(), 5u);
    EXPECT_EQ(result.lines[0], "line_995");
    EXPECT_EQ(result.lines[4], "line_999");
}

TEST_F(TailTest, TailLines_Basic) {
    std::vector<std::string> input = {"a", "b", "c", "d", "e"};
    
    auto result = tail_lines(input, 2);
    
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.lines.size(), 2u);
    EXPECT_EQ(result.lines[0], "d");
    EXPECT_EQ(result.lines[1], "e");
}

} // namespace
} // namespace tail


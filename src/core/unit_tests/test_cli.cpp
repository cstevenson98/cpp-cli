#include "cli.hpp"

#include <gtest/gtest.h>

namespace cli {
namespace {

class CliExecutorTest : public ::testing::Test {
protected:
    void SetUp() override {
        executor = std::make_unique<CliExecutor>("test_program", "A test program");
    }
    
    std::unique_ptr<CliExecutor> executor;
};

// Flag parsing tests

TEST_F(CliExecutorTest, ParseFlagNames_ShortAndLong) {
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Verbose output");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-v"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--verbose"));
    
    result = executor->parse({"cmd", "--verbose"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--verbose"));
}

TEST_F(CliExecutorTest, ParseFlagNames_LongOnly) {
    executor->add_flag("--debug", FlagType::Boolean, "Debug mode");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "--debug"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--debug"));
}

TEST_F(CliExecutorTest, ParseFlagNames_ShortOnly) {
    executor->add_flag("-d", FlagType::Boolean, "Debug mode");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-d"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("-d"));
}

// Multi-argument flag tests

TEST_F(CliExecutorTest, MultiArgFlag_ShortForm_SpaceSeparated) {
    executor->add_flag("-f,--files", FlagType::MultiArg, "Input files");
    executor->add_command("process", "Process files", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"process", "-f", "file1.txt", "file2.txt", "file3.txt"});
    EXPECT_TRUE(result.success);
    
    auto files = result.get_args("--files");
    ASSERT_EQ(files.size(), 3u);
    EXPECT_EQ(files[0], "file1.txt");
    EXPECT_EQ(files[1], "file2.txt");
    EXPECT_EQ(files[2], "file3.txt");
}

TEST_F(CliExecutorTest, MultiArgFlag_LongForm_CommaSeparated) {
    executor->add_flag("-f,--files", FlagType::MultiArg, "Input files");
    executor->add_command("process", "Process files", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"process", "--files=file1.txt,file2.txt,file3.txt"});
    EXPECT_TRUE(result.success);
    
    auto files = result.get_args("--files");
    ASSERT_EQ(files.size(), 3u);
    EXPECT_EQ(files[0], "file1.txt");
    EXPECT_EQ(files[1], "file2.txt");
    EXPECT_EQ(files[2], "file3.txt");
}

TEST_F(CliExecutorTest, MultiArgFlag_LongForm_SpaceSeparated) {
    executor->add_flag("-f,--files", FlagType::MultiArg, "Input files");
    executor->add_command("process", "Process files", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"process", "--files", "file1.txt", "file2.txt"});
    EXPECT_TRUE(result.success);
    
    auto files = result.get_args("--files");
    ASSERT_EQ(files.size(), 2u);
    EXPECT_EQ(files[0], "file1.txt");
    EXPECT_EQ(files[1], "file2.txt");
}

TEST_F(CliExecutorTest, MultiArgFlag_SingleValue) {
    executor->add_flag("-o,--output", FlagType::MultiArg, "Output file");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-o", "output.txt"});
    EXPECT_TRUE(result.success);
    
    auto output = result.get_args("--output");
    ASSERT_EQ(output.size(), 1u);
    EXPECT_EQ(output[0], "output.txt");
}

// Command tests

TEST_F(CliExecutorTest, CommandExecution) {
    int callback_result = 0;
    executor->add_command("run", "Run something", [&callback_result](const ParseResult&) {
        callback_result = 42;
        return 0;
    });
    
    auto result = executor->parse({"run"});
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.command, "run");
    
    executor->execute(result);
    EXPECT_EQ(callback_result, 42);
}

TEST_F(CliExecutorTest, UnknownCommand) {
    executor->add_command("known", "Known command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"unknown"});
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error_message.find("Unknown command") != std::string::npos);
}

TEST_F(CliExecutorTest, UnknownFlag) {
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "--unknown"});
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error_message.find("Unknown flag") != std::string::npos);
}

// Command-specific flags

TEST_F(CliExecutorTest, CommandSpecificFlag) {
    executor->add_command("build", "Build project", [](const ParseResult&) { return 0; });
    executor->add_command_flag("build", "-j,--jobs", FlagType::MultiArg, "Number of jobs");
    
    auto result = executor->parse({"build", "-j", "4"});
    EXPECT_TRUE(result.success);
    
    auto jobs = result.get_args("--jobs");
    ASSERT_EQ(jobs.size(), 1u);
    EXPECT_EQ(jobs[0], "4");
}

// Required flags

TEST_F(CliExecutorTest, RequiredFlag_Present) {
    executor->add_flag("-i,--input", FlagType::MultiArg, "Input file", true);
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-i", "file.txt"});
    EXPECT_TRUE(result.success);
}

TEST_F(CliExecutorTest, RequiredFlag_Missing) {
    executor->add_flag("-i,--input", FlagType::MultiArg, "Input file", true);
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd"});
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error_message.find("Missing required flag") != std::string::npos);
}

// Positional arguments

TEST_F(CliExecutorTest, PositionalArguments) {
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Verbose");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "arg1", "arg2", "-v", "arg3"});
    EXPECT_TRUE(result.success);
    
    ASSERT_EQ(result.positional_args.size(), 3u);
    EXPECT_EQ(result.positional_args[0], "arg1");
    EXPECT_EQ(result.positional_args[1], "arg2");
    EXPECT_EQ(result.positional_args[2], "arg3");
    EXPECT_TRUE(result.get_bool("--verbose"));
}

// Mixed flags

TEST_F(CliExecutorTest, MixedFlags) {
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Verbose");
    executor->add_flag("-f,--files", FlagType::MultiArg, "Files");
    executor->add_flag("-o,--output", FlagType::MultiArg, "Output");
    executor->add_command("process", "Process files", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({
        "process", 
        "-v", 
        "-f", "in1.txt", "in2.txt",
        "--output=out.txt"
    });
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--verbose"));
    
    auto files = result.get_args("--files");
    ASSERT_EQ(files.size(), 2u);
    EXPECT_EQ(files[0], "in1.txt");
    EXPECT_EQ(files[1], "in2.txt");
    
    auto output = result.get_args("--output");
    ASSERT_EQ(output.size(), 1u);
    EXPECT_EQ(output[0], "out.txt");
}

// Empty args

TEST_F(CliExecutorTest, EmptyArgs) {
    auto result = executor->parse({});
    EXPECT_FALSE(result.success);
}

// Help flag

TEST_F(CliExecutorTest, HelpFlag) {
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "--help"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--help"));
}

// has_flag method

TEST_F(CliExecutorTest, HasFlag) {
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Verbose");
    executor->add_flag("-f,--files", FlagType::MultiArg, "Files");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-v"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.has_flag("--verbose"));
    EXPECT_FALSE(result.has_flag("--files"));
}

// get_bool returns false for MultiArg

TEST_F(CliExecutorTest, GetBool_ReturnseFalseForMultiArg) {
    executor->add_flag("-f,--files", FlagType::MultiArg, "Files");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-f", "file.txt"});
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.get_bool("--files"));
}

// get_args returns empty for Boolean

TEST_F(CliExecutorTest, GetArgs_ReturnsEmptyForBoolean) {
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Verbose");
    executor->add_command("cmd", "Test command", [](const ParseResult&) { return 0; });
    
    auto result = executor->parse({"cmd", "-v"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_args("--verbose").empty());
}

// Help generation

TEST_F(CliExecutorTest, HelpGeneration) {
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Enable verbose output");
    executor->add_command("build", "Build the project", [](const ParseResult&) { return 0; });
    executor->add_command("test", "Run tests", [](const ParseResult&) { return 0; });
    
    auto help_text = executor->help();
    
    EXPECT_TRUE(help_text.find("test_program") != std::string::npos);
    EXPECT_TRUE(help_text.find("build") != std::string::npos);
    EXPECT_TRUE(help_text.find("test") != std::string::npos);
    EXPECT_TRUE(help_text.find("--verbose") != std::string::npos);
}

TEST_F(CliExecutorTest, CommandHelpGeneration) {
    executor->add_command("build", "Build the project", [](const ParseResult&) { return 0; });
    executor->add_command_flag("build", "-j,--jobs", FlagType::MultiArg, "Parallel jobs");
    
    auto help_text = executor->help("build");
    
    EXPECT_TRUE(help_text.find("build") != std::string::npos);
    EXPECT_TRUE(help_text.find("--jobs") != std::string::npos);
    EXPECT_TRUE(help_text.find("Parallel jobs") != std::string::npos);
}

} // namespace
} // namespace cli


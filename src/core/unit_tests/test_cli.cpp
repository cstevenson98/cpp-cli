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

// Nested command tests

TEST_F(CliExecutorTest, NestedCommand_TwoLevels) {
    executor->add_nested_command("git.status", "Show git status", 
        [](const ParseResult& result) {
            return 10;
        });
    
    auto result = executor->parse({"git", "status"});
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.command_path.size(), 2u);
    EXPECT_EQ(result.command_path[0], "git");
    EXPECT_EQ(result.command_path[1], "status");
    EXPECT_EQ(result.command, "status");
    
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 10);
}

TEST_F(CliExecutorTest, NestedCommand_ThreeLevels) {
    executor->add_nested_command("docker.container.ls", "List containers", 
        [](const ParseResult& result) {
            return 20;
        });
    
    auto result = executor->parse({"docker", "container", "ls"});
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.command_path.size(), 3u);
    EXPECT_EQ(result.command_path[0], "docker");
    EXPECT_EQ(result.command_path[1], "container");
    EXPECT_EQ(result.command_path[2], "ls");
    
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 20);
}

TEST_F(CliExecutorTest, NestedCommand_FourLevels) {
    executor->add_nested_command("advent.year-2025.day-1.part-1", "AoC 2025 Day 1 Part 1", 
        [](const ParseResult& result) {
            return 42;
        });
    
    auto result = executor->parse({"advent", "year-2025", "day-1", "part-1"});
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.command_path.size(), 4u);
    EXPECT_EQ(result.command_path[0], "advent");
    EXPECT_EQ(result.command_path[1], "year-2025");
    EXPECT_EQ(result.command_path[2], "day-1");
    EXPECT_EQ(result.command_path[3], "part-1");
    
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 42);
}

TEST_F(CliExecutorTest, NestedCommand_WithFlags) {
    executor->add_nested_command("app.deploy.prod", "Deploy to production", 
        [](const ParseResult& result) {
            return result.get_bool("--force") ? 1 : 0;
        });
    
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Verbose output");
    
    auto result = executor->parse({"app", "deploy", "prod", "-v"});
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--verbose"));
    ASSERT_EQ(result.command_path.size(), 3u);
    
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 0);
}

TEST_F(CliExecutorTest, NestedCommand_WithNestedFlags) {
    executor->add_nested_command("app.deploy.staging", "Deploy to staging", 
        [](const ParseResult& result) {
            auto version = result.get_args("--version");
            return version.empty() ? 0 : 5;
        });
    
    executor->add_nested_command_flag("app.deploy.staging", "-v,--version", 
                                      FlagType::MultiArg, "Version to deploy");
    
    auto result = executor->parse({"app", "deploy", "staging", "--version", "1.2.3"});
    EXPECT_TRUE(result.success);
    
    auto version = result.get_args("--version");
    ASSERT_EQ(version.size(), 1u);
    EXPECT_EQ(version[0], "1.2.3");
    
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 5);
}

TEST_F(CliExecutorTest, NestedCommand_WithPositionalArgs) {
    executor->add_nested_command("aws.s3.cp", "Copy files to S3", 
        [](const ParseResult& result) {
            return result.positional_args.size();
        });
    
    auto result = executor->parse({"aws", "s3", "cp", "source.txt", "dest.txt"});
    EXPECT_TRUE(result.success);
    ASSERT_EQ(result.positional_args.size(), 2u);
    EXPECT_EQ(result.positional_args[0], "source.txt");
    EXPECT_EQ(result.positional_args[1], "dest.txt");
    
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 2);
}

TEST_F(CliExecutorTest, NestedCommand_MultipleSubcommands) {
    executor->add_nested_command("git.branch.list", "List branches", 
        [](const ParseResult&) { return 1; });
    executor->add_nested_command("git.branch.create", "Create branch", 
        [](const ParseResult&) { return 2; });
    executor->add_nested_command("git.branch.delete", "Delete branch", 
        [](const ParseResult&) { return 3; });
    
    auto result1 = executor->parse({"git", "branch", "list"});
    EXPECT_TRUE(result1.success);
    EXPECT_EQ(executor->execute(result1), 1);
    
    auto result2 = executor->parse({"git", "branch", "create"});
    EXPECT_TRUE(result2.success);
    EXPECT_EQ(executor->execute(result2), 2);
    
    auto result3 = executor->parse({"git", "branch", "delete"});
    EXPECT_TRUE(result3.success);
    EXPECT_EQ(executor->execute(result3), 3);
}

TEST_F(CliExecutorTest, NestedCommand_UnknownSubcommand) {
    executor->add_nested_command("docker.container.ls", "List containers", 
        [](const ParseResult&) { return 0; });
    
    // docker is known, but docker unknown is not
    auto result = executor->parse({"docker", "container", "unknown"});
    EXPECT_TRUE(result.success);
    // Should stop at "container" level and treat "unknown" as positional arg
    ASSERT_EQ(result.command_path.size(), 2u);
    ASSERT_EQ(result.positional_args.size(), 1u);
    EXPECT_EQ(result.positional_args[0], "unknown");
}

TEST_F(CliExecutorTest, NestedCommand_HelpForTopLevel) {
    executor->add_nested_command("kubectl.get.pods", "Get pods", 
        [](const ParseResult&) { return 0; });
    executor->add_nested_command("kubectl.get.services", "Get services", 
        [](const ParseResult&) { return 0; });
    
    auto help_text = executor->help(std::vector<std::string>{"kubectl"});
    
    EXPECT_TRUE(help_text.find("kubectl") != std::string::npos);
    EXPECT_TRUE(help_text.find("Subcommands:") != std::string::npos);
    EXPECT_TRUE(help_text.find("get") != std::string::npos);
}

TEST_F(CliExecutorTest, NestedCommand_HelpForMidLevel) {
    executor->add_nested_command("kubectl.get.pods", "Get pods", 
        [](const ParseResult&) { return 0; });
    executor->add_nested_command("kubectl.get.services", "Get services", 
        [](const ParseResult&) { return 0; });
    
    auto help_text = executor->help(std::vector<std::string>{"kubectl", "get"});
    
    EXPECT_TRUE(help_text.find("kubectl get") != std::string::npos);
    EXPECT_TRUE(help_text.find("pods") != std::string::npos);
    EXPECT_TRUE(help_text.find("services") != std::string::npos);
}

TEST_F(CliExecutorTest, NestedCommand_HelpForLeafCommand) {
    executor->add_nested_command("kubectl.get.pods", "Get pods", 
        [](const ParseResult&) { return 0; });
    executor->add_nested_command_flag("kubectl.get.pods", "-n,--namespace", 
                                      FlagType::MultiArg, "Kubernetes namespace");
    
    auto help_text = executor->help(std::vector<std::string>{"kubectl", "get", "pods"});
    
    EXPECT_TRUE(help_text.find("kubectl get pods") != std::string::npos);
    EXPECT_TRUE(help_text.find("Get pods") != std::string::npos);
    EXPECT_TRUE(help_text.find("--namespace") != std::string::npos);
}

TEST_F(CliExecutorTest, NestedCommand_MixedFlagsAndArgs) {
    executor->add_nested_command("tool.sub.action", "Perform action", 
        [](const ParseResult&) { return 0; });
    
    executor->add_flag("-v,--verbose", FlagType::Boolean, "Global verbose");
    executor->add_nested_command_flag("tool.sub.action", "-o,--output", 
                                      FlagType::MultiArg, "Output file");
    
    auto result = executor->parse({
        "tool", "sub", "action", 
        "-v", 
        "--output=out.txt",  // Use = syntax to avoid consuming positional args
        "arg1", "arg2"
    });
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.get_bool("--verbose"));
    
    auto output = result.get_args("--output");
    ASSERT_EQ(output.size(), 1u);
    EXPECT_EQ(output[0], "out.txt");
    
    ASSERT_EQ(result.positional_args.size(), 2u);
    EXPECT_EQ(result.positional_args[0], "arg1");
    EXPECT_EQ(result.positional_args[1], "arg2");
}

TEST_F(CliExecutorTest, NestedCommand_NoCallbackShowsHelp) {
    executor->add_nested_command("parent.child.leaf", "Leaf command", 
        [](const ParseResult&) { return 99; });
    
    // Parse only to parent.child (which has no callback, only subcommands)
    auto result = executor->parse({"parent", "child"});
    EXPECT_TRUE(result.success);
    
    // Execute should show help and return 0
    int exit_code = executor->execute(result);
    EXPECT_EQ(exit_code, 0);
}

} // namespace
} // namespace cli


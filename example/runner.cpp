#include "runner.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <Qbe.h>
#include "spdlog/spdlog.h"

int runner::run() {
        auto temp_file_path = std::filesystem::temp_directory_path() / "qbe_output.qbe";
    std::ofstream temp_file(temp_file_path);
    if (!temp_file) {
        spdlog::error("Failed to open temporary file for writing: {}", temp_file_path.string());
        return 1;
    }
    temp_file << output;
    temp_file.close();

    // Now run qbe qbe_output.qbe -o qbe_output.S
    std::string command = "qbe " + temp_file_path.string() + " -o " + (temp_file_path.parent_path() / "qbe_output.S").string();
    int result = std::system(command.c_str());
    if (result != 0) {
        spdlog::error("Failed to run qbe command: {}", command);
        return 1;
    }
    spdlog::info("QBE output written to: {}", temp_file_path.string());
    spdlog::info("Assembly output written to: {}", (temp_file_path.parent_path() / "qbe_output.S").string());

    // Read in the generated assembly file and print
    std::ifstream assembly_file(temp_file_path.parent_path() / "qbe_output.S");
    if (!assembly_file) {
        spdlog::error("Failed to open assembly file: {}", temp_file_path.string());
        return 1;
    }

    spdlog::info("Generated assembly code:");
    Utilities::StringBuilder sb;
    std::string line;
    while (std::getline(assembly_file, line)) {
        sb.AppendLine(line);
    }
    assembly_file.close();
    spdlog::info("\n{}", sb.ToString());

    // Run it using clang -o qbe_output qbe_output.S
    command = "clang -o " + (temp_file_path.parent_path() / "qbe_output").string() + " " + (temp_file_path.parent_path() / "qbe_output.S").string() + " -O2 -fno-stack-protector -fno-pie -no-pie";
    spdlog::info("Compiling assembly output with command: {}", command);
    result = std::system(command.c_str());
    if (result != 0) {
        spdlog::error("Failed to compile assembly output: {}", command);
        return 1;
    }

    // Now run the compiled output
    command = (temp_file_path.parent_path() / "qbe_output").string();
    result = std::system(command.c_str()) / 256; // Divide by 256 to get the exit code

    spdlog::info("Execution of compiled output finished with result: {}", result);
    return 0;
}

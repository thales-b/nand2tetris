#include "compilation_engine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

void compileJackFile(const std::string& inputFileName) {
    std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + ".vm";

    std::ifstream inputFile;
    inputFile.open(inputFileName);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputFileName << std::endl;
        return;
    }

    std::ofstream outputFile;
    outputFile.open(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to create output file: " << outputFileName << std::endl;
        return;
    }

    CompilationEngine compilationEngine(inputFile, outputFile);

    compilationEngine.compileClass();

    std::cout << "Compilation completed successfully. Output written to: " << outputFileName << std::endl;

    inputFile.close();
    outputFile.close();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.jack or directory>" << std::endl;
        return 1;
    }

    std::string inputPath(argv[1]);

    if (std::filesystem::is_directory(inputPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".jack") {
                compileJackFile(entry.path().string());
            }
        }
    } else if (inputPath.length() >= 5 && inputPath.substr(inputPath.size() - 5) == ".jack") {
        compileJackFile(inputPath);
    } else {
        std::cerr << "Input must be a .jack file or a directory containing .jack files." << std::endl;
        return 1;
    }

    return 0;
}

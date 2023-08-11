#include "parser.h"
#include "code.h"
#include <iostream>
#include <fstream>
#include <bitset>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " input_file.asm" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];

    if (filePath.find(".asm") == std::string::npos) {
        std::cerr << "Input file must have .asm extension." << std::endl;
        return 1;
    }

    Parser parser(filePath);

    std::ofstream hackFile(filePath.substr(0, filePath.find(".asm")) + ".hack");
    if (!hackFile.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return 1;
    }
    
    //initializes current command allowing function call at end of loop
    //so the predicate applies to the current command 
    parser.advance();

    while (parser.hasMoreCommands()) {
        if (parser.commandType() == A_COMMAND) {
            hackFile << "0";
            int symbolValue = std::stoi(parser.symbol());
            std::bitset<15> binarySymbol(symbolValue);
            hackFile << binarySymbol << "\n";
        }

        else if (parser.commandType() == C_COMMAND) {
            hackFile << "111";
            hackFile << comp(parser.comp());
            hackFile << dest(parser.dest());
            hackFile << jump(parser.jump()) << "\n";
        }
        parser.advance();
    }

    hackFile.close();

    return 0;
}

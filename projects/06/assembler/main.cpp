#include "parser.h"
#include "code.h"
#include "symbol_table.h"
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

    parser.mapLabels();
    //initializes current command allowing function call at end of loop
    //so the predicate applies to the current command 
    parser.advance();

    while (parser.hasMoreCommands()) {
        std::string symbol = parser.symbol();

        if (parser.commandType() == A_COMMAND) {
            if (isStringInteger(symbol))
            {
                int symbolValue = std::stoi(symbol);
                std::bitset<15> binarySymbol(symbolValue);
                hackFile << "0" << binarySymbol << "\n";
            }
            else if (parser.symbolTable.contains(symbol))
            {
                std::bitset<15> binarySymbol(parser.symbolTable.getAddress(symbol));
                hackFile << "0" << binarySymbol << "\n";
            }
            else
            {
                int ramAddress = parser.getRamAddress();
                parser.symbolTable.addEntry(symbol, ramAddress);
                std::bitset<15> binaryRamAddress(ramAddress);
                hackFile << "0" << binaryRamAddress << "\n";
            }
        }

        else if (parser.commandType() == C_COMMAND) {
            hackFile << "111" << comp(parser.comp()) << dest(parser.dest()) 
                    << jump(parser.jump()) << "\n";
        }

        parser.advance();
    }

    hackFile.close();

    return 0;
}

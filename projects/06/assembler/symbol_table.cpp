#include "symbol_table.h"
#include <map>
#include <unordered_map>

SymbolTable::SymbolTable()
{
    symbolTable.insert({"SP", 0});
    symbolTable.insert({"LCL", 1});
    symbolTable.insert({"ARG", 2});
    symbolTable.insert({"THIS", 3});
    symbolTable.insert({"THAT", 4});
    for (int i = 0; i < 16; ++i) {
        symbolTable.insert({"R" + std::to_string(i), i});
    }    
    symbolTable.insert({"SCREEN", 16384});
    symbolTable.insert({"KBD", 24576});
}

void SymbolTable::addEntry(std::string symbol, int address)
{
    symbolTable.insert({symbol, address});
}

bool SymbolTable::contains(std::string symbol)
{
    return symbolTable.count(symbol);
}

int SymbolTable::getAddress(std::string symbol)
{
    return symbolTable[symbol];
}

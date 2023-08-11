#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <map>

class SymbolTable
{
    public:
        SymbolTable();
        void addEntry(std::string symbol, int address);
        bool contains(std::string symbol);
        int getAddress(std::string symbol);
    private:
        std::unordered_map<std::string, int> symbolTable;
};

#endif
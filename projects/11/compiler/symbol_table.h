#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <map>
#include <unordered_map>
#include "jack_tokenizer.h"

enum class Kind {
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
};

class SymbolTable {
    public:
        SymbolTable();
        void startSubroutine();
        void define(const std::string& name, const std::string& type, Kind kind);
        int varCount(Kind kind);
        Kind kindOf(const std::string& name);
        std::string typeOf(const std::string& name);
        int indexOf(const std::string& name);
        Kind kind(const std::string& kind);
        std::string segment(Kind kind);
    private:
        std::unordered_map<std::string, std::tuple<std::string, Kind, int>> classScope;
        std::unordered_map<std::string, std::tuple<std::string, Kind, int>> subroutineScope;
};

#endif
#include "symbol_table.h"

SymbolTable::SymbolTable() {

}

void SymbolTable::startSubroutine() {
    subroutineScope.clear();
}

void SymbolTable::define(const std::string& name, const std::string& type, Kind kind) {
    int index = varCount(kind);
    if (kind == Kind::STATIC || kind == Kind::FIELD) {
        classScope[name] = std::make_tuple(type, kind, index);
    } else {
        subroutineScope[name] = std::make_tuple(type, kind, index);
    }
}

int SymbolTable::varCount(Kind kind) {
    int count = 0;
    if (kind == Kind::STATIC || kind == Kind::FIELD) {
        for (const auto& entry : classScope) {
            if (std::get<1>(entry.second) == kind) {
                count++;
            }
        }
    } else {
        for (const auto& entry : subroutineScope) {
            if (std::get<1>(entry.second) == kind) {
                count++;
            }
        }
    }
    return count;
}

Kind SymbolTable::kindOf(const std::string& name) {
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return std::get<1>(subroutineScope[name]);
    } else if (classScope.find(name) != classScope.end()) {
        return std::get<1>(classScope[name]);
    }
    return Kind::NONE;
}

std::string SymbolTable::typeOf(const std::string& name) {
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return std::get<0>(subroutineScope[name]);
    } else if (classScope.find(name) != classScope.end()) {
        return std::get<0>(classScope[name]);
    }
    return ""; 
}

int SymbolTable::indexOf(const std::string& name) {
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return std::get<2>(subroutineScope[name]);
    } else if (classScope.find(name) != classScope.end()) {
        return std::get<2>(classScope[name]);
    }
    return -1; 
}

Kind SymbolTable::kind(const std::string& kind)
{
    if (kind == "static") return Kind::STATIC;
    else if (kind == "field") return Kind::FIELD;
    else if (kind == "var") return Kind::VAR;
    else if (kind == "arg") return Kind::ARG;
    return Kind::NONE;
}

std::string SymbolTable::segment(Kind kind)
{
    if (kind == Kind::STATIC) return "static";
    else if (kind == Kind::VAR) return "local";
    else if (kind == Kind::ARG) return "argument";
    else if (kind == Kind::FIELD) return "this";
    return "";
}
#ifndef PARSER_H
#define PARSER_H

#include <fstream> 
#include <string>

enum CommandType
{
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

class Parser
{
    public:
        Parser(std::string filePath);
        bool hasMoreCommands();
        void advance();
        CommandType commandType();
        std::string arg1();
        int arg2();
    private:
        std::ifstream inputFile;
        std::string currCommand = "";
        void trimCurrCommand();
        std::string get1stOr2ndArg(int argNo, int argsStart);
};

#endif 
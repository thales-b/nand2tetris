#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>

enum TokenType {
    KEYWORD, SYMBOL,
    IDENTIFIER, INT_CONST,
    STRING_CONST
};

enum KeyWord {
    CLASS, METHOD, FUNCTION,
    CONSTRUCTOR, INT,
    BOOLEAN, CHAR, VOID,
    VAR, STATIC, FIELD, LET,
    DO, IF, ELSE, WHILE,
    RETURN, TRUE, FALSE,
    MY_NULL, THIS
};

const std::vector<char> symbols = {
    '{', '}', '(', ')', '[', ']', '.',
    ',', ';', '+', '-', '*', '/', '&',
    '|', '<', '>', '=', '~'
};

class JackTokenizer
{
    public:
        JackTokenizer(std::ifstream& inputFile);
        bool hasMoreTokens();
        void advance();
        TokenType tokenType();
        std::string keyWord();
        char symbol();
        std::string identifier();
        int intVal();
        std::string stringVal();
    private:
        std::ifstream& inputFile;
        std::string currToken = "";
        bool stringConstant = false;
};

#endif
#include "jack_tokenizer.h"
#include <iostream>
#include <algorithm>

JackTokenizer::JackTokenizer(std::ifstream& inputFile) : inputFile(inputFile)
{

}

bool JackTokenizer::hasMoreTokens()
{
    return !inputFile.eof();
}

void JackTokenizer::advance()
{
    // resets predicate
    stringConstant = false;
    currToken = "";

    while (hasMoreTokens())
    {
        char nextChar;
        inputFile.get(nextChar);

        if (nextChar == '/')
        {
            char peekChar = inputFile.peek();
            if (peekChar == '/')
            {
                // ignore the rest of the line as a comment
                std::string line;
                std::getline(inputFile, line);
            }
            else if (peekChar == '*')
            {
                inputFile.ignore(1); // consume the '*' character
                while (true)
                {
                    char currentChar = inputFile.get();
                    char nextChar = inputFile.peek();
                    if (currentChar == '*' && nextChar == '/')
                    {
                        inputFile.ignore(1); // consume the '/' character
                        break;
                    }
                }
            }

            else
            {
                currToken = "/";
                break;
            }
        }
        else if (nextChar == '\"')
        {
            stringConstant = true;
            while (true)
            {
                char currentChar = inputFile.get();
                if (currentChar == '\"')
                {
                    break;
                }
                currToken += currentChar;
            }
            break;
        }
        else if (std::isspace(nextChar))
        {
            continue;
        }
        else if (std::find(symbols.begin(), symbols.end(), nextChar) != symbols.end())        
        {
            // handle standalone symbols
            currToken = nextChar;
            break;
        }
        else
        {
            // keywords, identifiers, numbers
            currToken = nextChar;
            while (hasMoreTokens())
            {
                char peekChar = inputFile.peek();
                if (std::isspace(peekChar) || std::find(symbols.begin(), symbols.end(), peekChar) != symbols.end())
                {
                    break;
                }

                currToken += inputFile.get();
            }
            break;        
        }
    }
}

TokenType JackTokenizer::tokenType()
{
    const std::vector<std::string> keywords = {
        "class", "constructor", "function", "method",
        "field", "static", "var", "int", "char", "boolean",
        "void", "true", "false", "null", "this", "let",
        "do", "if", "else", "while", "return"
    };
    auto keyword = std::find(keywords.begin(), keywords.end(), currToken);
    if (keyword != keywords.end()) return KEYWORD; 

    auto symbol = std::find(symbols.begin(), symbols.end(), currToken[0]);
    if (symbol != symbols.end()) return SYMBOL; 

    if (stringConstant)
    {
        return STRING_CONST;
    }

    bool isInt = true;
    for (char c : currToken)
    {
        if (!std::isdigit(c))
        {
            isInt = false;
            break;
        }
    }
    if (isInt) return INT_CONST;

    return IDENTIFIER;
}

std::string JackTokenizer::keyWord()
{
    return currToken;
}

char JackTokenizer::symbol()
{
    return currToken.empty() ? '\0' : currToken[0];
}

std::string JackTokenizer::identifier()
{
    return currToken;
}

int JackTokenizer::intVal()
{
    return currToken.empty() ? 0 : std::stoi(currToken);
}

std::string JackTokenizer::stringVal()
{
    return currToken;
}

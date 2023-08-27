#include "parser.h"
#include <iostream>
#include <algorithm>
#include <array>

Parser::Parser(std::string filePath) : inputFile(filePath) 
{
    if (!inputFile.is_open())
    {
        std::cerr << "Couldn't open file" << filePath << std::endl;
    }
}

bool Parser::hasMoreCommands()
{
    return !inputFile.eof();
}

void Parser::trimCurrCommand()
{
	size_t commentStart = currCommand.find("//");
	if (commentStart != std::string::npos)
	{
		currCommand = currCommand.substr(0, commentStart);
	}

    currCommand.erase(std::remove_if(currCommand.begin(), currCommand.end(), 
                                      [](char c) { return (c == '\n' || c == '\r'); }),
									  currCommand.end());
}

void Parser::advance()
{
    std::getline(inputFile, currCommand);
    trimCurrCommand();
    
    while (hasMoreCommands() && currCommand.empty())
    {
        std::getline(inputFile, currCommand);
        trimCurrCommand();
    }
}

CommandType Parser::commandType()
{
    std::array<std::string, 9> arithmeticCommands = {
        "add", "sub", "neg", "eq", "gt",
        "lt", "and", "or", "not" 
    };
    auto arithmetic = std::find_if(arithmeticCommands.begin(), arithmeticCommands.end(),
        [&](const std::string& command) {
            return currCommand.find(command) == 0;
        });

    if (arithmetic != arithmeticCommands.end()) {
        return C_ARITHMETIC;
    }
    if (currCommand.substr(0, 4) == "push")
    {
        return C_PUSH;
    }
    if (currCommand.substr(0, 3) == "pop")
    {
        return C_POP;
    }
    if (currCommand.substr(0, 5) == "label")
    {
        return C_LABEL;
    }
    if (currCommand.substr(0, 4) == "goto")
    {
        return C_GOTO;
    }    
    if (currCommand.substr(0, 7) == "if-goto")
    {
        return C_IF;
    }
    if (currCommand.substr(0, 8) == "function")
    {
        return C_FUNCTION;
    }    
    if (currCommand.substr(0, 5) == "label")
    {
        return C_LABEL;
    }
    if (currCommand.substr(0, 6) == "return")
    {
        return C_RETURN;
    }
    throw std::invalid_argument("Invalid command");
}

std::string Parser::get1stOr2ndArg(int argNo, int argsStart)
{
    std::string args = currCommand.substr(argsStart);
    size_t space = args.find(' ');
    if (space == std::string::npos)
    {
        return args;
    }
    if (argNo == 1)
    {
        return args.substr(0, space);
    }
    else 
    {
        return args.substr(space + 1);
    }
}

std::string Parser::arg1()
{
    if (commandType() != C_RETURN)
    {
        if (commandType() == C_ARITHMETIC)
        {
            return currCommand;
        }
        if (commandType() == C_PUSH || commandType() == C_GOTO
            || commandType() == C_CALL)
        {
            return get1stOr2ndArg(1, currCommand.find(' ') + 1);
        }
        if (commandType() == C_POP)
        {
            return get1stOr2ndArg(1, currCommand.find(' ') + 1);

        }
        if (commandType() == C_LABEL)
        {
            return currCommand.substr(6);
        }
        if (commandType() == C_IF)
        {
            return currCommand.substr(8);
        }   
        if (commandType() == C_FUNCTION)
        {
            return get1stOr2ndArg(1, currCommand.find(' ') + 1);

        } 
    }

    return "";
}

int Parser::arg2()
{
    if (commandType() == C_PUSH || commandType() == C_POP
        || commandType() == C_FUNCTION || commandType() == C_CALL)
    {
        return std::stoi(get1stOr2ndArg(2, currCommand.find(' ') + 1));
    }

    return -1;
}

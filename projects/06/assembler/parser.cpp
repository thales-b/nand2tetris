#include "parser.h"
#include <algorithm>
#include <iostream>

Parser::Parser(std::string filePath) : inputFile(filePath)
{
	if (!inputFile.is_open())
	{
		std::cerr << "Could not open file" << std::endl;
	}
}

bool Parser::hasMoreCommands() { 
	return !inputFile.eof(); }

//erases comments, whitespaces, and special characters (blank lines)
void Parser::trimCurrCommand()
{
	size_t commentStart = currCommand.find("//");
	if (commentStart != std::string::npos)
	{
		currCommand = currCommand.substr(0, commentStart);
	}

    currCommand.erase(std::remove_if(currCommand.begin(), currCommand.end(), 
                                      [](char c) { return std::isspace(c); }),
									  currCommand.end());
}

void Parser::advance()
{
	std::getline(inputFile, currCommand);
	trimCurrCommand();
	//if empty command, continues reading until eof or actual command 
	while (currCommand.empty() && hasMoreCommands())
	{
		std::getline(inputFile, currCommand);
		trimCurrCommand();		
	} 
}

CommandType Parser::commandType()
{
	if (currCommand[0] == '@')
	{
		return A_COMMAND;
	}

	if (currCommand[0] == '(')
	{
		return L_COMMAND;
	}

	return C_COMMAND;
}

std::string Parser::symbol() 
{
	if (commandType() == A_COMMAND) 
	{
		return currCommand.substr(1);
	}

	if (commandType() == L_COMMAND)
	{
		return currCommand.substr(1, currCommand.length() - 1);
	}

	return "";
}

std::string Parser::dest() 
{
	size_t equalPos = currCommand.find('=');
	
	if (commandType() == C_COMMAND 
	&& equalPos != std::string::npos) 
	{
		return currCommand.substr(0, equalPos);
	}
	
	return "";
}

std::string Parser::comp() 
{
	size_t equalPos = currCommand.find('=');
	size_t semicolonPos = currCommand.find(';');
	
	if (commandType() == C_COMMAND) 
	{
		if (equalPos != std::string::npos 
		&& semicolonPos != std::string::npos)
		{
			return currCommand.substr(equalPos + 1, semicolonPos);
		}	

		if (equalPos == std::string::npos 
		&& semicolonPos != std::string::npos)
		{
			return currCommand.substr(0, semicolonPos);
		}	

		if (equalPos != std::string::npos 
		&& semicolonPos == std::string::npos)
		{
			return currCommand.substr(equalPos + 1);
		}	

		if (equalPos == std::string::npos 
		&& semicolonPos == std::string::npos) 
		{
			return currCommand;
		}
	}

	return "";
}

std::string Parser::jump() 
{
	size_t semicolonPos = currCommand.find(';');
	
	if (commandType() == C_COMMAND 
	&& semicolonPos != std::string::npos) 
	{
		return currCommand.substr(semicolonPos + 1);
	}
	
	return "";
}

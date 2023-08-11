#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>

enum CommandType
{
  A_COMMAND,
  C_COMMAND,
  L_COMMAND
};

class Parser
{
public:
  Parser(std::string filePath);
  bool hasMoreCommands();
  void advance();
  CommandType commandType();
  std::string symbol();
  std::string dest();
  std::string comp();
  std::string jump();

private:
  std::ifstream inputFile;
  std::string currCommand = "";
  void trimCurrCommand();
};

#endif

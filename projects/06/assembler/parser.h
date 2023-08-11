#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>
#include "symbol_table.h"

enum CommandType
{
  A_COMMAND,
  C_COMMAND,
  L_COMMAND
};

bool isStringInteger(std::string string);

class Parser
{
public:
  Parser(std::string filePath);
  void rewindInputFile();
  void mapLabels();
  bool hasMoreCommands();
  int getRamAddress();
  void advance();
  CommandType commandType();
  std::string symbol();
  std::string dest();
  std::string comp();
  std::string jump();
  SymbolTable symbolTable;

private:
  std::ifstream inputFile;
  std::string currCommand;
  void trimCurrCommand();
  int ramAddress = 16;
};

#endif

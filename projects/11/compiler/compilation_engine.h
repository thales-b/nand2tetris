#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <string>
#include "jack_tokenizer.h"
#include "symbol_table.h"
#include "vm_writer.h"

class CompilationEngine
{
    public:
        CompilationEngine(std::ifstream& inputFile, std::ofstream& outputFile);
        void compileClass();
        void compileClassVarDec();
        void compileSubroutine();
        void compileParameterList();
        void compileVarDec();
        void compileStatements();
        void compileDo();
        void compileLet();
        void compileWhile();
        void compileReturn();
        void compileIf();
        void compileExpression();
        void compileTerm();
        int compileExpressionList();
        void compileSubroutineCall(std::string);
    private:
        std::ofstream outputFile;
        JackTokenizer tokenizer;
        SymbolTable table;
        VMWriter writer;
        void advance();
        bool isOperator();
        bool isKeywordConstant();
        bool isUnaryOp();
        std::string className;
        int labelCounter;
        void writeKeywordConstant();
        void writeOperator(char op);
        void writeUnaryOp(char op);
};

#endif
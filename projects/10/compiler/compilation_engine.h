#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <string>
#include "jack_tokenizer.h"

class CompilationEngine
{
    public:
        CompilationEngine(std::string inputName, std::string outputName);
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
        void compileExpressionList();
    private:
        std::ofstream outputFile;
        JackTokenizer tokenizer;
        void advance();
        void writeNonTerminal(const std::string &element);
        void writeTerminal();
        bool isOperator();
        bool isKeywordConstant();
        bool isUnaryOp();
};

#endif
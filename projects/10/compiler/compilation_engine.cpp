#include "compilation_engine.h"
#include <iostream>

CompilationEngine::CompilationEngine(std::string inputName, std::string outputName)
    : outputFile(outputName), tokenizer(inputName) {
    if (!outputFile.is_open()) {
        std::cout << "failed to open file" << std::endl;
    }
}

void CompilationEngine::advance() {
    tokenizer.advance();
}

void CompilationEngine::writeTerminal() {
    if (tokenizer.tokenType() == IDENTIFIER) {
        outputFile << "<identifier> " << tokenizer.identifier() << " </identifier>\n";
    } else if (tokenizer.tokenType() == SYMBOL) {
        outputFile << "<symbol> " << tokenizer.symbol() << " </symbol>\n";
    } else if (tokenizer.tokenType() == KEYWORD) {
        outputFile << "<keyword> " << tokenizer.keyWord() << " </keyword>\n";
    } else if (tokenizer.tokenType() == INT_CONST) {
        outputFile << "<integerConstant> " << tokenizer.intVal() << " </integerConstant>\n";
    } else if (tokenizer.tokenType() == STRING_CONST) {
        outputFile << "<stringConstant> " << tokenizer.stringVal() << " </stringConstant>\n";
    }
}

void CompilationEngine::compileClass() {
    outputFile << "<tokens>\n";

    advance();

    writeTerminal(); // write 'class' keyword
    advance();

    writeTerminal(); // write class name
    advance();

    writeTerminal(); // write '{'
    advance();

    while (tokenizer.keyWord() == "static" || tokenizer.keyWord() == "field") {
        compileClassVarDec();
        advance(); // move past ';'
    }

    while (tokenizer.keyWord() == "constructor" || tokenizer.keyWord() == "function" || tokenizer.keyWord() == "method") {
        compileSubroutine();
        advance();
    }

    writeTerminal(); // write '}'

    outputFile << "</tokens>\n";
}

void CompilationEngine::compileClassVarDec() {
    writeTerminal(); // write 'static' or 'field'
    advance();

    writeTerminal(); // write variable type
    advance();

    writeTerminal(); // write variable name
    advance();

    while (tokenizer.symbol() == ',') {
        writeTerminal(); // write ','
        advance();

        writeTerminal(); // write variable name
        advance();
    }

    writeTerminal(); // write ';'
}

void CompilationEngine::compileSubroutine() {
    writeTerminal(); // write 'constructor', 'function', or 'method'
    advance();

    writeTerminal(); // write return type
    advance();

    writeTerminal(); // write subroutine name
    advance();

    writeTerminal(); // write '('
    advance();

    compileParameterList();

    writeTerminal(); // write ')'
    advance();

    writeTerminal(); // write '{'
    advance();

    while (tokenizer.keyWord() == "var") {
        compileVarDec();
        advance(); // move past ';'
    }

    compileStatements();

    writeTerminal(); // write '}'
}

void CompilationEngine::compileParameterList() {
    if (tokenizer.symbol() != ')')
    {
        writeTerminal(); // write parameter type
        advance();

        writeTerminal(); // write parameter name
        advance();

        while (tokenizer.symbol() == ',') {
            writeTerminal(); // write ','
            advance();

            writeTerminal(); // write parameter type
            advance();

            writeTerminal(); // write parameter name
            advance();
        }
    }
}

void CompilationEngine::compileVarDec() {
    writeTerminal(); // write 'var'
    advance();

    writeTerminal(); // write variable type
    advance();

    writeTerminal(); // write variable name
    advance();

    while (tokenizer.symbol() == ',') {
        writeTerminal(); // write ','
        advance();
        
        writeTerminal(); // write variable name
        advance();
    }

    writeTerminal(); // write ';'
}

// end with ';' or '}' 
void CompilationEngine::compileStatements() {
    while (true) {
        if (tokenizer.keyWord() == "let") {
            compileLet();
            advance(); // move past ';'
        } else if (tokenizer.keyWord() == "do") {
            compileDo();
            advance(); // move past ';'
        } else if (tokenizer.keyWord() == "if") {
            compileIf(); 
            //advance(); // move past '}'
        } else if (tokenizer.keyWord() == "while") {
            compileWhile();
            advance(); // move past '}'
        } else if (tokenizer.keyWord() == "return") {
            compileReturn();
            advance(); // move past ';'
        } else {
            break; 
        }
    }
}

void CompilationEngine::compileLet()
{
    writeTerminal(); // write 'let' keyword
    advance();
    
    writeTerminal(); // write identifier
    advance();
    
    if (tokenizer.symbol() == '[')
    {
        writeTerminal(); // write '['
        advance();
        
        compileExpression();

        writeTerminal(); // write ']'
        advance();
    }

    writeTerminal(); // write '='
    advance();

    compileExpression();

    writeTerminal(); // write ';'
}

void CompilationEngine::compileIf()
{
    writeTerminal(); // write 'if' keyword
    advance();
    
    writeTerminal(); // write '('
    advance();

    compileExpression();

    writeTerminal(); // write ')'
    advance();

    writeTerminal(); // write '{'
    advance();

    compileStatements();

    writeTerminal(); // write '}'
    advance();

    if (tokenizer.keyWord() == "else")
    {
        writeTerminal(); // write 'else' keyword
        advance();

        writeTerminal(); // write '{'
        advance();

        compileStatements();

        writeTerminal(); // write '}'
        advance();
    }
}

void CompilationEngine::compileWhile()
{
    writeTerminal(); // write 'while' keyword
    advance();

    writeTerminal(); // write '('
    advance();

    compileExpression();

    writeTerminal(); // write ')'
    advance();

    writeTerminal(); // write '{'
    advance();

    compileStatements();

    writeTerminal(); // write '}'
}

void CompilationEngine::compileDo()
{
    writeTerminal(); // write 'do' keyword
    advance();

    writeTerminal(); // write identifier
    advance();

    if (tokenizer.symbol() == '.')
    {
        writeTerminal(); // write '.'
        advance();

        writeTerminal(); // write identifier
        advance();
    }

    writeTerminal(); // write '('
    advance();

    compileExpressionList();

    writeTerminal(); // write ')'
    advance();

    writeTerminal(); // Write ';'

}

void CompilationEngine::compileExpressionList()
{
    if (tokenizer.symbol() != ')')
    {
        compileExpression();

        while (tokenizer.symbol() == ',')
        {
            writeTerminal(); // write ','
            advance();

            compileExpression();
        }
    }
}

void CompilationEngine::compileReturn() {
    writeTerminal(); // write 'return' keyword
    advance();

    if (tokenizer.symbol() != ';') {
        compileExpression();
    }

    writeTerminal(); // write ';'
}

void CompilationEngine::compileExpression() {
    compileTerm();

    while (isOperator()) {
        writeTerminal(); // write operator
        advance();

        compileTerm();
    }
}

// ends with operator or ';' or identifier or ']' or ')'
void CompilationEngine::compileTerm() {
    if (tokenizer.tokenType() == INT_CONST) {
        writeTerminal(); // write integer constant
        advance(); // move past constant
    } 
    else if (tokenizer.tokenType() == STRING_CONST) 
    {
        writeTerminal(); // write string constant
        advance(); // move past constant
    } 
    else if (isKeywordConstant()) 
    {
        writeTerminal(); // write keyword constant
        advance(); // move past constant
    } 
    else if (tokenizer.tokenType() == IDENTIFIER) 
    {
        writeTerminal(); // write identifier
        advance();

        if (tokenizer.symbol() == '[') 
        {
            writeTerminal(); // write '['
            advance();

            compileExpression();

            writeTerminal(); // write ']'
            advance();
        } 
        else if (tokenizer.symbol() == '(' || tokenizer.symbol() == '.') 
        {

            if (tokenizer.symbol() == '.')
            {
                writeTerminal(); // write '.'
                advance();

                writeTerminal(); // write identifier
                advance();
            }

            writeTerminal(); // write '('
            advance();

            compileExpressionList();

            writeTerminal(); // write ')'
            advance();
        }
    }
    else if (tokenizer.symbol() == '(') {
        writeTerminal(); // write '('
        advance();

        compileExpression();

        writeTerminal(); // write ')'
        advance();
    } 
    else if (isUnaryOp()) {
        writeTerminal(); // write unary operator
        advance();

        compileTerm();
    }
}

bool CompilationEngine::isOperator() {
    char c = tokenizer.symbol();
    return (c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '&' || c == '|' || c == '<' || c == '>' || c == '=');
}

bool CompilationEngine::isKeywordConstant() {
    std::string keyword = tokenizer.keyWord();
    return (keyword == "true" || keyword == "false" || keyword == "null" || keyword == "this");
}

bool CompilationEngine::isUnaryOp() {
    char c = tokenizer.symbol();
    return (c == '-' || c == '~');
}

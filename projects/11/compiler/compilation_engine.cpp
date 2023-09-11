#include "compilation_engine.h"
#include <iostream>

CompilationEngine::CompilationEngine(std::ifstream& inputFile, std::ofstream& outputFile)
    : tokenizer(inputFile), writer(outputFile), table(), labelCounter(0), className("")
{

}

void CompilationEngine::advance() {
    tokenizer.advance();
}

void CompilationEngine::compileClass() {
    advance();

    // 'class' keyword
    advance(); 

    className = tokenizer.keyWord();
    advance();

    // '{'
    advance();

    while (tokenizer.keyWord() == "static" || tokenizer.keyWord() == "field") {
        compileClassVarDec();
        advance(); // move past ';'
    }

    while (tokenizer.keyWord() == "constructor" || tokenizer.keyWord() == "function" || tokenizer.keyWord() == "method") {
        compileSubroutine();
        advance();
    }

    // '}'
}

void CompilationEngine::compileClassVarDec() {
    std::string kind = tokenizer.keyWord(); 
    advance();

    std::string type = tokenizer.keyWord();
    advance();

    std::string name = tokenizer.keyWord();
    advance();

    table.define(name, type, table.kind(kind));

    while (tokenizer.symbol() == ',') {
        advance(); // ','

        name = tokenizer.keyWord();

        table.define(name, type, table.kind(kind)); 
        advance();
    }

    // ';'
}

void CompilationEngine::compileSubroutine() {
    table.startSubroutine();

    std::string subroutineType = tokenizer.keyWord(); // 'constructor', 'function', or 'method'
    advance();

    // return type
    advance();

    std::string subroutineName = className + "." + tokenizer.keyWord(); 
    advance();

    // '('
    advance();

    if (subroutineType == "method")
    {
        table.define("this", className, Kind::ARG);
    }
    compileParameterList();

    // ')'
    advance();

    // '{'
    advance();

    while (tokenizer.keyWord() == "var") {
        compileVarDec();
        advance(); // move past ';'
    }

    int numLocals = table.varCount(Kind::VAR);
    writer.writeFunction(subroutineName, numLocals);

    if (subroutineType == "method")
    {
        writer.writePush("argument", 0);
        writer.writePop("pointer", 0);
    }
    else if (subroutineType == "constructor")
    {
        writer.writePush("constant", table.varCount(Kind::FIELD));
        writer.writeCall("Memory.alloc", 1);
        writer.writePop("pointer", 0);
    }

    compileStatements();

    // '}'
}

void CompilationEngine::compileParameterList() {
    if (tokenizer.symbol() != ')')
    {
        std::string type = tokenizer.keyWord(); // write parameter type
        advance();

        std::string name = tokenizer.keyWord(); // write parameter name
        advance();

        table.define(name, type, Kind::ARG);    

        while (tokenizer.symbol() == ',') { 
            advance(); // ','

            std::string type = tokenizer.keyWord(); // write parameter type
            advance();

            std::string name = tokenizer.keyWord(); // write parameter name
            advance();

            table.define(name, type, Kind::ARG); 
        }
    }
}

void CompilationEngine::compileVarDec() {
    std::string kind = tokenizer.keyWord(); 
    advance();

    std::string type = tokenizer.keyWord();
    advance();

    std::string name = tokenizer.keyWord();
    advance();

    table.define(name, type, table.kind(kind));

    while (tokenizer.symbol() == ',') {
        advance(); // ','

        name = tokenizer.keyWord();

        table.define(name, type, table.kind(kind)); 
        advance();
    }

    // ';'
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
    // 'let' keyword
    advance();
    
    std::string identifier = tokenizer.keyWord(); // identifier
    advance();
    
    bool hasBrackets = false;
    if (tokenizer.symbol() == '[')
    {
        hasBrackets = true;
        writer.writePush(table.segment(table.kindOf(identifier)), table.indexOf(identifier));

        // '['
        advance();
        
        compileExpression();

        // ']'
        advance();

        writer.writeArithmetic("add");
    }

    // '='
    advance();

    compileExpression();

    if (!hasBrackets)
    {
        writer.writePop(table.segment(table.kindOf(identifier)), table.indexOf(identifier));
    }
    else
    {
        writer.writePop("temp", 0);
        writer.writePop("pointer", 1);
        writer.writePush("temp", 0);
        writer.writePop("that", 0);
    }
    // ';'
}

void CompilationEngine::compileIf()
{
    std::string l1 = "L" + std::to_string(labelCounter);
    labelCounter++;
    std::string l2 = "L" + std::to_string(labelCounter);
    labelCounter++;

    // 'if' keyword
    advance();
    
    // '('
    advance();

    compileExpression();

    writer.writeArithmetic("not");

    // ')'
    advance();

    writer.writeIf(l1);

    // '{'
    advance();

    compileStatements();

    // '}'
    advance();

    if (tokenizer.keyWord() == "else")
    {
        writer.writeGoto(l2);

        // 'else' keyword
        advance();

        writer.writeLabel(l1);

        // '{'
        advance();

        compileStatements();

        // '}'
        advance();

        writer.writeLabel(l2);
    }
    else 
    {
        writer.writeLabel(l1);
    }
}

void CompilationEngine::compileWhile()
{
    std::string l1 = "L" + std::to_string(labelCounter);
    labelCounter++;
    std::string l2 = "L" + std::to_string(labelCounter);
    labelCounter++;

    writer.writeLabel(l1); // 'while' keyword
    advance();

    // '('
    advance();

    compileExpression();

    writer.writeArithmetic("not");

    // ')'
    advance();

    writer.writeIf(l2);

    // '{'
    advance();

    compileStatements();

    writer.writeGoto(l1);

    // '}'

    writer.writeLabel(l2);
}

void CompilationEngine::compileDo()
{
    // 'do' keyword
    advance();

    std::string identifier = tokenizer.keyWord(); // identifier
    advance();

    compileSubroutineCall(identifier);
    writer.writePop("temp", 0);
}

// Returns the number of arguments in a function call.
int CompilationEngine::compileExpressionList()
{
    if (tokenizer.symbol() != ')')
    {
        compileExpression();
        int numArgs = 1;

        while (tokenizer.symbol() == ',')
        {
            // ','
            advance();

            compileExpression();
            numArgs++;
        }

        return numArgs;
    }

    return 0;
}

void CompilationEngine::compileReturn() {
    // 'return' keyword
    advance();

    if (tokenizer.symbol() != ';') {
        compileExpression();
    }

    else 
    {
        writer.writePush("constant", 0);
    }

    writer.writeReturn();

    // ';'
}

void CompilationEngine::compileExpression() {
    compileTerm();

    while (isOperator()) {
        char op = tokenizer.symbol(); // store operator
        advance();

        compileTerm();

        writeOperator(op);
    }
}

void CompilationEngine::compileSubroutineCall(std::string identifier)
{
    bool isMethod = false;
    std::string methodName = "";
    if (tokenizer.symbol() == '.')
    {
        isMethod = true;
        // '.'
        advance();

        methodName = tokenizer.keyWord(); // identifier
        advance();
    }
    else
    {
        methodName = identifier;
    }

    // '('
    advance();

    if (isMethod && table.typeOf(identifier) != "")
    {
        writer.writePush(table.segment(table.kindOf(identifier)),
                            table.indexOf(identifier));
    }
    else if (!isMethod)
    {
        writer.writePush("pointer", 0);
    }

    int numArgs = compileExpressionList();

    // ')'
    advance();

    std::string objectMethodName = "";
    if (isMethod)
    {
        std::string callerName = "";
        // OS subroutine
        if (table.typeOf(identifier) == "")
        {
            callerName = identifier;
        }
        // object method
        else
        {
            callerName = table.typeOf(identifier);
            numArgs++;
        }
        objectMethodName = callerName + "." + methodName;
        writer.writeCall(objectMethodName, numArgs);
    }
    // void method
    else
    {
        objectMethodName = this->className + "." + methodName;
        numArgs++;
        writer.writeCall(objectMethodName, numArgs);
    }
}

// ends with operator or ';' or identifier or ']' or ')'
void CompilationEngine::compileTerm() {
    if (tokenizer.tokenType() == INT_CONST) {
        writer.writePush("constant", tokenizer.intVal()); // integer constant
        advance(); // move past constant
    } 
    else if (tokenizer.tokenType() == STRING_CONST) 
    {
        // string constant
        std::string stringConstant = tokenizer.stringVal();
        writer.writePush("constant", stringConstant.length());
        writer.writeCall("String.new", 1);
        for (int i = 0; i < stringConstant.length(); i++)
        {
            writer.writePush("constant", (int) stringConstant[i]);
            writer.writeCall("String.appendChar", 2);
        }

        advance(); // move past constant
    } 
    else if (isKeywordConstant()) 
    {
        writeKeywordConstant(); // keyword constant
        advance(); // move past constant
    } 
    else if (tokenizer.tokenType() == IDENTIFIER) 
    {
        std::string identifier = tokenizer.keyWord(); // identifier
        advance();

        if (tokenizer.symbol() == '[') 
        {
            writer.writePush(table.segment(table.kindOf(identifier)), table.indexOf(identifier));

            // '['
            advance();

            compileExpression();

            // ']'
            advance();

            writer.writeArithmetic("add");
            
            writer.writePop("pointer", 1);
            writer.writePush("that", 0);
        } 
        else if (tokenizer.symbol() == '(' || tokenizer.symbol() == '.') 
        {
            compileSubroutineCall(identifier);
        }
        else
        {
            writer.writePush(table.segment(table.kindOf(identifier)), table.indexOf(identifier));
        }
    }
    else if (tokenizer.symbol() == '(') {
        // '('
        advance();

        compileExpression();

        // ')'
        advance();
    } 
    else if (isUnaryOp()) {
        char op = tokenizer.symbol(); // unary operator
        advance();

        compileTerm();

        writeUnaryOp(op);
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

void CompilationEngine::writeKeywordConstant()
{
    std::string keyword = tokenizer.keyWord();
    if (keyword == "true") 
    {
        writer.writePush("constant", 1);
        writer.writeArithmetic("neg");
    }
    else if (keyword == "false" || keyword == "null") writer.writePush("constant", 0);
    else writer.writePush("pointer", 0);
}

bool CompilationEngine::isUnaryOp() {
    char c = tokenizer.symbol();
    return (c == '-' || c == '~');
}

void CompilationEngine::writeUnaryOp(char op) {
    if (op == '-') writer.writeArithmetic("neg");
    else writer.writeArithmetic("not");
}

void CompilationEngine::writeOperator(char op)
{
    if (op == '+')
    {
        writer.writeArithmetic("add");
    }
    else if (op == '-')
    {
        writer.writeArithmetic("sub");
    }
    else if (op == '*')
    {
        writer.writeCall("Math.multiply", 2); 
    }
    else if (op == '/')
    {
        writer.writeCall("Math.divide", 2);  
    }
    else if (op == '&')
    {
        writer.writeArithmetic("and");
    }
    else if (op == '|')
    {
        writer.writeArithmetic("or");
    }
    else if (op == '<')
    {
        writer.writeArithmetic("lt");
    }
    else if (op == '>')
    {
        writer.writeArithmetic("gt");
    }
    else if (op == '=')
    {
        writer.writeArithmetic("eq");
    }
}

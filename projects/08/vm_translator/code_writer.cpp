#include "code_writer.h"
#include <iostream>

CodeWriter::CodeWriter(std::string fileName)
{
    outputFile.open(fileName.substr(0, fileName.rfind(".vm")) + ".asm");
    if (!outputFile.is_open()) {
        std::cerr << "Couldn't open file" << std::endl;
    }
}

void CodeWriter::setFileName(std::string fileName) {
    this->fileName = fileName.substr(fileName.rfind('/') + 1, fileName.rfind(".vm") - fileName.rfind('/') - 1);
    std::cout << "Currently translating " << fileName << std::endl;
}

void CodeWriter::writeInit()
{
    outputFile << "//***************************************\n";
    outputFile << "// Init\n";
    outputFile << "//***************************************\n";
    outputFile << "@256\n";
    outputFile << "D = A\n";
    outputFile << "@SP\n";
    outputFile << "M = D\n";
    writeCall("Sys.init", 0);
}

void CodeWriter::close()
{
    outputFile.close();
    if (!outputFile.good())
    {
        std::cerr << "Couldn't close file" << std::endl;
    }
}

void CodeWriter::writeUnaryArithmetic(std::string operation)
{
    outputFile << "//***************************************\n";
    outputFile << "// Unary arithmetic\n";
    outputFile << "//***************************************\n";
    outputFile << "@SP\n";
    outputFile << "M = M - 1\n";
    outputFile << "A = M\n";
    outputFile << "D = " << operation << "M\n"; // perform unary operation
    writePush();
}

void CodeWriter::writeBinaryArithmetic(std::string operation)
{
    outputFile << "//***************************************\n";
    outputFile << "// Binary arithmetic\n";
    outputFile << "//***************************************\n";
    outputFile << "@SP\n";
    outputFile << "M = M - 1\n"; //stack pointer on second value 
    outputFile << "A = M\n";
    outputFile << "D = M\n";
    outputFile << "A = A - 1\n"; // go to previous address
    outputFile << "D = M " << operation << " D\n"; // perform binary operation
    outputFile << "M = D\n";
}

void CodeWriter::writeBooleanArithmetic(std::string jumpType)
{
    outputFile << "//***************************************\n";
    outputFile << "// Boolean arithmetic\n";
    outputFile << "//***************************************\n";
    writePop();
    outputFile << "A = A - 1\n";
    outputFile << "D = M - D\n";
    outputFile << "@TRUE" << labelCounter << "\n";
    outputFile << "D; " << jumpType << "\n"; // goto true if condition met
    outputFile << "@SP\n";
    outputFile << "A = M - 1\n";
    outputFile << "M = 0\n"; // push false
    outputFile << "@END" << labelCounter << "\n";
    outputFile << "0; JMP\n"; // goto end
    outputFile << "(TRUE" << labelCounter << ")\n";
    outputFile << "@SP\n";
    outputFile << "A = M - 1\n";
    outputFile << "M = -1\n"; // push true
    outputFile << "(END" << labelCounter << ")\n";
    outputFile << "@SP\n";
    labelCounter++;
}

void CodeWriter::writeArithmetic(std::string command)
{
    if (command == "add")
    {
        writeBinaryArithmetic("+");
    }

    if (command == "sub")
    {
        writeBinaryArithmetic("-");
    }

    if (command == "neg")
    {
        writeUnaryArithmetic("-");
    }

    if (command == "eq")
    {
        writeBooleanArithmetic("JEQ");
    }
    
    if (command == "gt")
    {
        writeBooleanArithmetic("JGT");
    }

    if (command == "lt")
    {
        writeBooleanArithmetic("JLT");
    }

    if (command == "and")
    {
        writeBinaryArithmetic("&");
    }

    if (command == "or")
    {
        writeBinaryArithmetic("|");
    }

    if (command == "not")
    {
        writeUnaryArithmetic("!");
    }
}

void CodeWriter::writePush()
{
    outputFile << "@SP\n";
    outputFile << "A = M \n"; 
    outputFile << "M = D\n";
    outputFile << "@SP\n";
    outputFile << "M = M + 1\n";
}

void CodeWriter::writePop()
{
    outputFile << "@SP\n";
    outputFile << "M = M - 1\n";   
    outputFile << "A = M\n"; 
    outputFile << "D = M\n";   
}

void CodeWriter::loadSegmentAddress(std::string segment, int index)
    {
        if (segment == "constant")
        {
            outputFile << "@" << index << "\n";
            outputFile << "D = A\n";
        }
        else if (segment == "static")
        {
            outputFile << "@" << this->fileName << "." << index << "\n";
        }
        else if (segment == "pointer")
        {
            outputFile << "@" << ((index + THIS) < 4 ? "THIS" : "THAT") << "\n";
        }
        else if (segment == "temp")
        {
            outputFile << "@" << TEMP + index << "\n";
        }
        else
        {
            outputFile << "@" << SEGMENT.at(segment) << "\n";
            outputFile << "D = M\n";
            outputFile << "@" << index << "\n";
            outputFile << "D = D + A\n"; // fetches source/target address
        }
}

void CodeWriter::writePushPop(CommandType command, std::string segment, int index, bool pointer)
{
    if (command == C_PUSH)
    {
        outputFile << "// Write push\n";
        loadSegmentAddress(segment, index);
        if (!pointer && segment != "constant" && segment != "pointer" && segment != "static" && segment != "temp")
        {
            outputFile << "A = D\n"; // loads fetched address
        }
        if (!pointer && segment != "constant")
        {
            outputFile << "D = M\n";
        }
        writePush();
    }

    else if (command == C_POP)
    {
        outputFile << "// Write pop\n";
        if (segment == "pointer" || segment == "static" || segment == "temp")
        {
            writePop();
            loadSegmentAddress(segment, index);
            outputFile << "M = D\n"; // store popped address into segment
        }
        else
        {
            loadSegmentAddress(segment, index);
            outputFile << "@R13\n"; 
            outputFile << "M = D\n";
            writePop();
            outputFile << "@R13\n";
            outputFile << "A = M\n";
            outputFile << "M = D\n";
        }
    }
}

void CodeWriter::writeLabel(std::string label)
{
    outputFile << "(" << label << ")\n";
}

void CodeWriter::writeGoto(std::string label)
{
    outputFile << "@" << label << "\n";
    outputFile << "0; JMP\n";
}

void CodeWriter::writeIf(std::string label)
{
    outputFile << "//***************************************\n";
    outputFile << "// If-goto\n";
    outputFile << "//***************************************\n";
    outputFile << "@SP\n";
    outputFile << "M = M - 1\n";
    outputFile << "A = M\n";
    outputFile << "D = M\n";
    outputFile << "@" << label << "\n";
    outputFile << "D; JNE\n";
}

void CodeWriter::writeCall(std::string functionName, int numArgs)
{
    outputFile << "//***************************************\n";
    outputFile << "// Call\n";
    outputFile << "//***************************************\n";
    returnCounter++;
    outputFile << "@returnAddress" << returnCounter << "\n";
    outputFile << "D = A\n";
    writePush();
    writePushPop(C_PUSH, "local", 0, true);
    writePushPop(C_PUSH, "argument", 0, true);
    writePushPop(C_PUSH, "this", 0, true);
    writePushPop(C_PUSH, "that", 0, true);
    outputFile << "// ARG = SP - (nArgs + 5)\n";
    outputFile << "@SP\n";
    outputFile << "D = M\n";
    outputFile << "@" << 5 + numArgs << "\n";
    outputFile << "D = D - A\n";
    outputFile << "@ARG\n";
    outputFile << "M = D\n"; 
    outputFile << "// LCL = SP\n";
    outputFile << "@SP\n";
    outputFile << "D = M\n";
    outputFile << "@LCL\n";
    outputFile << "M = D\n"; 
    writeGoto(functionName);
    writeLabel("returnAddress" + std::to_string(returnCounter));
}

void CodeWriter::writeFunction(std::string functionName, int numLocals)
{
    outputFile << "//***************************************\n";
    outputFile << "// Function\n";
    outputFile << "//***************************************\n";
    int count = 0;
    writeLabel(functionName);  
    while (count < numLocals)
    {
        writePushPop(C_PUSH, "constant", 0);
        count++;
    }
}

void CodeWriter::restoreSegment(std::string segment, int offset)
{    
    outputFile << "@FRAME\n";    
    outputFile << "D = M\n";
    outputFile << "@" << offset << "\n";
    outputFile << "D = D - A\n";
    outputFile << "A = D\n";
    outputFile << "D = M\n";
    outputFile << "@" << segment << "\n";
    outputFile << "M = D\n";
}

void CodeWriter::writeReturn()
{
    outputFile << "//***************************************\n";
    outputFile << "// Return\n";
    outputFile << "//***************************************\n";
    outputFile << "// FRAME = LCL\n";
    outputFile << "@LCL\n";
    outputFile << "D = M\n";
    outputFile << "@FRAME\n";
    outputFile << "M = D\n"; 
    outputFile << "// RET = *(FRAME - 5)\n";
    restoreSegment("RET", 5); 
    outputFile << "// *ARG = pop\n";
    writePushPop(C_POP, "argument", 0); 
    outputFile << "// SP = ARG + 1\n";
    outputFile << "@ARG\n";
    outputFile << "D = M + 1\n";
    outputFile << "@SP\n";
    outputFile << "M = D\n"; 
    outputFile << "// THAT = *(FRAME - 1)\n";
    restoreSegment("THAT", 1); 
    outputFile << "// THIS = *(FRAME - 2)\n";
    restoreSegment("THIS", 2); 
    outputFile << "// ARG = *(FRAME - 3)\n";
    restoreSegment("ARG", 3);
    outputFile << "// LCL = *(FRAME - 4)\n";
    restoreSegment("LCL", 4);
    outputFile << "@RET\n";
    outputFile << "A = M\n";
    outputFile << "0; JMP\n";
}

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
    std::cout << "Currently writing " << this->fileName << ".asm..." << std::endl;
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
    outputFile << "@SP\n";
    outputFile << "M = M - 1\n";
    outputFile << "A = M\n";
    outputFile << "D = " << operation << "M\n"; // perform unary operation
    writePush();
}

void CodeWriter::writeBinaryArithmetic(std::string operation)
{
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
            outputFile << "@" << index + THIS << "\n";
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

void CodeWriter::writePushPop(CommandType command, std::string segment, int index)
{
    if (command == C_PUSH)
    {
        loadSegmentAddress(segment, index);
        if (segment != "constant" && segment != "pointer" && segment != "static" && segment != "temp")
        {
            outputFile << "A = D\n"; // loads fetched address
        }
        if (segment != "constant")
        {
            outputFile << "D = M\n";
        }
        writePush();
    }

    else if (command == C_POP)
    {
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
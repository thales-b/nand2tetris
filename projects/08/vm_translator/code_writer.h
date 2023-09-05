#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include "parser.h"

const std::unordered_map<std::string, std::string> SEGMENT = 
    {
        {"local", "LCL"},
        {"argument", "ARG"},
        {"this", "THIS"},
        {"that", "THAT"}  
    };

const int THIS = 3;

const int TEMP = 5;

class CodeWriter
{
    public:
        CodeWriter(std::string fileName);
        void setFileName(std::string fileName);
        void writeInit();
        void writeArithmetic(std::string command);
        void writePushPop(CommandType command, std::string segment, int index, bool pointer = false);
        void writeLabel(std::string label);
        void writeGoto(std::string label);
        void writeIf(std::string label);
        void writeCall(std::string functionName, int numArgs);
        void writeReturn();
        void writeFunction(std::string functionName, int numLocals);
        void close();
    private:
        std::ofstream outputFile;   
        size_t labelCounter = 0;
        void writeUnaryArithmetic(std::string operation);
        void writeBinaryArithmetic(std::string operation);
        void writeBooleanArithmetic(std::string jumpType);
        std::string fileName;
        void writePush();
        void writePop();
        void loadSegmentAddress(std::string segment, int index);
        size_t returnCounter = 0;
        void restoreSegment(std::string, int offset);
};

#endif
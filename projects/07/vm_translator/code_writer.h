#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include "parser.h"

const std::unordered_map<std::string, size_t> SEGMENT = 
    {
        {"local", 1},
        {"argument", 2},
        {"this", 3},
        {"that", 4}  
    };

const int THIS = 3;

const int TEMP = 5;

class CodeWriter
{
    public:
        CodeWriter(std::string fileName);
        void setFileName(std::string fileName);
        void writeArithmetic(std::string command);
        void writePushPop(CommandType command, std::string segment, int index);
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
};

#endif
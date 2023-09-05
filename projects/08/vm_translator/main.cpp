#include "parser.h"
#include "code_writer.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

void translate(std::string file, CodeWriter& codeWriter)
{
    Parser parser(file);
    codeWriter.setFileName(file);
    parser.advance();
    while (parser.hasMoreCommands())
    {
    switch (parser.commandType())
    {
        case C_ARITHMETIC:
            codeWriter.writeArithmetic(parser.arg1());
            break;

        case C_PUSH:
        case C_POP:
            codeWriter.writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
            break;

        case C_LABEL:
            codeWriter.writeLabel(parser.arg1());
            break;

        case C_GOTO:
            codeWriter.writeGoto(parser.arg1());
            break;

        case C_IF:
            codeWriter.writeIf(parser.arg1());
            break;

        case C_CALL:
            codeWriter.writeCall(parser.arg1(), parser.arg2());
            break;

        case C_FUNCTION:
            codeWriter.writeFunction(parser.arg1(), parser.arg2());
            break;

        case C_RETURN:
            codeWriter.writeReturn();
            break;
    }
        parser.advance();
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <*.vm file/directory path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::cout << path << std::endl;

    if (std::filesystem::is_regular_file(path))   
    {
        if (path.substr(path.length() - 3) == ".vm")
        {
            CodeWriter codeWriter(path.substr(0, path.length() - 3) + ".asm");
            codeWriter.writeInit();
            
            translate(argv[1], codeWriter);
            codeWriter.close();
        }
    } 
    else if (std::filesystem::is_directory(path)) {
        std::string outputFileName = std::filesystem::path(path).filename().string();
        CodeWriter codeWriter(outputFileName);
        codeWriter.writeInit();

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".vm") {
                translate(entry.path().string(), codeWriter);
            }
        }

        codeWriter.close();
    } else {
        std::cerr << "Path does not exist." << std::endl;
        return 1;
    }

    return 0;
}

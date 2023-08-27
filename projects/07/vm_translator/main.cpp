#include "parser.h"
#include "code_writer.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

void translate(std::string file)
{
    Parser parser(file);
    CodeWriter codeWriter(file);
    codeWriter.setFileName(file);
    parser.advance();
    while (parser.hasMoreCommands())
    {
        if (parser.commandType() == C_ARITHMETIC)
        {
            codeWriter.writeArithmetic(parser.arg1());
        }

        if (parser.commandType() == C_PUSH || parser.commandType() == C_POP)
        {
            codeWriter.writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
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
            translate(argv[1]);
        }
    } 
    else if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".vm") {
                translate(entry.path().string());
            }
        }
    } else {
        std::cerr << "Path does not exist." << std::endl;
        return 1;
    }

    return 0;
}

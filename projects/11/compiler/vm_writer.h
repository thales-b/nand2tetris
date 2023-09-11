#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <string>
#include <fstream>

class VMWriter {
public:
    VMWriter(std::ofstream& outputFile);    
    void writePush(const std::string& segment, int index);
    void writePop(const std::string& segment, int index);
    void writeArithmetic(const std::string& command);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& name, int nArgs);
    void writeFunction(const std::string& name, int nLocals);
    void writeReturn();
    void close();
    
private:
    std::ofstream& outputFile;
    void writeVMCommand(const std::string& command);
};

#endif
#include "vm_writer.h"
#include <iostream>

VMWriter::VMWriter(std::ofstream& outputFile) : outputFile(outputFile) {
}

void VMWriter::writePush(const std::string& segment, int index) {
    writeVMCommand("push " + segment + " " + std::to_string(index));
}

void VMWriter::writePop(const std::string& segment, int index) {
    writeVMCommand("pop " + segment + " " + std::to_string(index));
}

void VMWriter::writeArithmetic(const std::string& command) {
    writeVMCommand(command);
}

void VMWriter::writeLabel(const std::string& label) {
    writeVMCommand("label " + label);
}

void VMWriter::writeGoto(const std::string& label) {
    writeVMCommand("goto " + label);
}

void VMWriter::writeIf(const std::string& label) {
    writeVMCommand("if-goto " + label);
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
    writeVMCommand("call " + name + " " + std::to_string(nArgs));
}

void VMWriter::writeFunction(const std::string& name, int nLocals) {
    writeVMCommand("function " + name + " " + std::to_string(nLocals));
}

void VMWriter::writeReturn() {
    writeVMCommand("return");
}

void VMWriter::close() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

void VMWriter::writeVMCommand(const std::string& command) {
    outputFile << command << std::endl;
}

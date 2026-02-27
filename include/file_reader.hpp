#pragma once 
#include <string>
#include <stdexcept>
#include "fstream"
class FileReader{

    public:    
    std::string readFile(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);//set pointer at end of file
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t)file.tellg();//gets length of file cuz its telling where the pointer is
        std::string buffer{};//allocate exact length
        buffer.resize(fileSize);
        file.seekg(0);//resets pointer to start of file
        file.read(buffer.data(), fileSize);//fills up vector
        file.close();
        return buffer;
     }
};

#pragma once 
#include <filesystem>
#include "shaderc/shaderc.hpp"
#include <string>

class ShaderCompiler {
    //straight taken from the shaderc github examples
    shaderc::Compiler compiler{};
    shaderc::CompileOptions options{};
    public:
        std::vector<uint32_t> compileFile(const std::string &source_name, shaderc_shader_kind kind, const std::string &source, bool optimize){
            

            
            if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

            shaderc::SpvCompilationResult module =
                compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

            if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            // Print the actual GLSL compilation error to the console
            printf("shader Compilation failed: \n %s", module.GetErrorMessage().c_str());
            throw std::runtime_error("dont work shader");
        }
            return {module.cbegin(), module.cend()};
        }

//std::cerr << "Shader compilation failed:\n" << module.GetErrorMessage() << std::endl;
};
#pragma once 
#include <filesystem>
#include "shaderc/shaderc.hpp"
#include <string>
class ShaderCompiler {
    public:
        std::vector<uint32_t> compileFile(const std::string &source_name, shaderc_shader_kind kind, const std::string &source, bool optimize){
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;

            // Like -DMY_DEFINE=1
            options.AddMacroDefinition("MY_DEFINE", "1");
            if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

            shaderc::SpvCompilationResult module =
                compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

            if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                throw std::runtime_error("dont work shader");
                //return std::vector<uint32_t>();
            }

            return {module.cbegin(), module.cend()};
        }


};
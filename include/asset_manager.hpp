#pragma once
#include "stb_image.h"
#include "tinygltf.h"
#include "hash_model.hpp"
#include <filesystem>
#include "vertex_def.hpp"
class AssetManager{
public:
    struct imgInfo{
		
		stbi_uc* ptr;
		uint32_t texH;
		uint32_t texW;
	};

	struct UploadData {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		std::vector<ModelRecord> records{};

	};

	ModelStorage record{};

    imgInfo processImageFile(std::string_view path){
        
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(std::string(path).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels) {
			throw std::runtime_error(std::string("failed to load texture: "));
		}
		if (texChannels > 4){
			throw std::runtime_error("texChannel in processimage more than 4???");
		}
		
		return {pixels, uint32_t(texHeight) , uint32_t(texWidth)};
    }
		
	tinygltf::Model getModel(const std::string& filename){
		std::filesystem::path base = "models/"; 
		std::filesystem::path fullPath = base / filename;

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, fullPath.string());

		if (!warn.empty()) {
			std::cout << "GLTF WARN: " << warn << std::endl;
		}

		if (!ret) {
			std::cerr << "GLTF ERROR: " << err << std::endl;
			
			// // 4. CHECK FOR EXTENSION ERROR
			// if (err.find("extensionsRequired") != std::string::npos) {
			//     std::cerr << ">> FIX: Your GLB requires an extension (likely KHR_texture_basisu)." << std::endl;
			//     std::cerr << ">> Since your TinyGLTF is old, you must remove 'extensionsRequired' from the GLB using the Python script or update TinyGLTF." << std::endl;
			// }

			std::cerr << "Exiting..." << std::endl;
			exit(1);
		}
		
		return model;
	}

    
    void getData(tinygltf::Model &model,UploadData& mData);
};
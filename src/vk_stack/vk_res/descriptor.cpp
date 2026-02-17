

#include "vk_mmu.hpp"
void ResManager::initDescriptorPoolAndSets(vk::Device device, uint32_t maxImageAmount,uint32_t maxSamplers) {


	//bindless vars
	
	vk::DescriptorSetLayoutCreateInfo UBOSetLayout{};
	vk::DescriptorSetLayoutCreateInfo samplerSetLayout{};
	vk::DescriptorSetLayoutCreateInfo imageSetlayout{};
    
	//set type of descryptor, which stage used in, binding in shader code


	vk::DescriptorSetLayoutBinding samplerBind{};
	samplerBind
		.setBinding(0)
		.setDescriptorType(vk::DescriptorType::eSampler)
		.setDescriptorCount(maxSamplers)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment);

	vk::DescriptorSetLayoutBinding uboBind{};
	uboBind
		.setBinding(0)
		.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
		.setDescriptorCount(1)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
	
	vk::DescriptorSetLayoutBinding imageBind{};
	imageBind
		.setBinding(0)
		.setDescriptorType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(maxImageAmount)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment);

	
	vk::DescriptorBindingFlags imageBindFlag =
		vk::DescriptorBindingFlagBits::ePartiallyBound |
		vk::DescriptorBindingFlagBits::eUpdateAfterBind |
		vk::DescriptorBindingFlagBits::eVariableDescriptorCount;
        
    vk::DescriptorBindingFlags samplerBindFlag =
            vk::DescriptorBindingFlagBits::eUpdateAfterBind;

    vk::DescriptorBindingFlags noBindFlags{};
	


	vk::DescriptorSetLayoutBindingFlagsCreateInfo imgFlagCI{};
	imgFlagCI
		.setBindingFlags(imageBindFlag);

	
	vk::DescriptorSetLayoutBindingFlagsCreateInfo noFlagsCI{};
	
	vk::DescriptorSetLayoutCreateInfo setLayoutImg{};
    vk::DescriptorSetLayoutCreateInfo setLayoutSampler{};
	vk::DescriptorSetLayoutCreateInfo setLayoutUBO{};
	
	setLayoutImg
		.setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool)
		.setBindings(imageBind)
		.setPNext(&imgFlagCI);

	setLayoutUBO
		.setBindings(uboBind);
	
    setLayoutSampler
        .setBindings(samplerBind);

    descriptorSetLayouts[static_cast<size_t>(DescriptorSetType::SAMPLER)] = device.createDescriptorSetLayout(setLayoutSampler);
    descriptorSetLayouts[static_cast<size_t>(DescriptorSetType::IMAGE)] = device.createDescriptorSetLayout(setLayoutImg);
    descriptorSetLayouts[static_cast<size_t>(DescriptorSetType::UBO)] = device.createDescriptorSetLayout(setLayoutUBO);
	
	
	//NOW pool creation
	vk::DescriptorPoolCreateInfo poolInfo{};
	vk::DescriptorPoolSize uboSizes{};
	vk::DescriptorPoolSize samplerSizes{};
	vk::DescriptorPoolSize imageSizes{};


	//////////////////////////////////////////POOOOOOOOOOOOL
	uboSizes
		.setDescriptorCount(1)//how many of this type of descritpor pool fits
		.setType(vk::DescriptorType::eUniformBufferDynamic);

	samplerSizes
		.setDescriptorCount(maxSamplers)//how many of this type of descritpor pool fits
		.setType(vk::DescriptorType::eSampler);

    imageSizes
		.setDescriptorCount(maxImageAmount)//how many of this type of descritpor pool fits
		.setType(vk::DescriptorType::eSampledImage);

    


	std::array<vk::DescriptorPoolSize, 3> poolSizes = { uboSizes,samplerSizes ,imageSizes};

	poolInfo
		.setMaxSets(3)//how many "DESCRIPTOR SETS" can be allcoated maximum
		.setPoolSizes(poolSizes)
		.setFlags(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);//required for dynamic advantage of bindless textures

	descriptorPool = device.createDescriptorPool(poolInfo);

	

	vk::DescriptorSetAllocateInfo setAlloc{};
	vk::DescriptorSetVariableDescriptorCountAllocateInfo varCount{};
	varCount.setDescriptorSetCount(3).setPDescriptorCounts(descriptorSetVariableDescriptorCount.data());
	setAlloc
		.setDescriptorPool(descriptorPool)
		.setDescriptorSetCount(3)//how many "DESCRIPTOR SETS" we choose to allocate
		.setSetLayouts(descriptorSetLayouts).setPNext(&varCount);

	device.allocateDescriptorSets(&setAlloc,descriptorSets.data());
}


void ResManager::initAndUpdateSamplers(vk::Device device, float maxAnisotropy) {

	
	vk::SamplerCreateInfo samplerInfo{};

	samplerInfo
		.setMagFilter(vk::Filter::eLinear)
		.setMinFilter(vk::Filter::eLinear)
		.setAddressModeU(vk::SamplerAddressMode::eRepeat)
		.setAddressModeV(vk::SamplerAddressMode::eRepeat)
		.setAddressModeW(vk::SamplerAddressMode::eRepeat)
		.setAnisotropyEnable(vk::True)
		.setMaxAnisotropy(maxAnisotropy)
		.setBorderColor(vk::BorderColor::eIntOpaqueBlack)
		.setUnnormalizedCoordinates(vk::False)
		.setCompareEnable(vk::False)
		.setCompareOp(vk::CompareOp::eLessOrEqual)
		.setMipmapMode(vk::SamplerMipmapMode::eLinear)
		.setMipLodBias(0.0f)
		.setMinLod(0.0f)
		.setMaxLod(1.0f);

		samplers[static_cast<uint32_t>(SamplerType::TEXTURE)] = device.createSampler(samplerInfo);
		
	//DEPTH SAMPLER, INCREDIBLY MUCH CRITICAL INFORMATION LOCATED HERE, IF ADRESS MODES WERE eRepeat AS ABOVE, SHADOWMAPS WOULD KEEP REPEATING!!!!!!
	samplerInfo
		.setMagFilter(vk::Filter::eLinear)
		.setMinFilter(vk::Filter::eLinear)
		.setAddressModeU(vk::SamplerAddressMode::eClampToBorder)
		.setAddressModeV(vk::SamplerAddressMode::eClampToBorder)
		.setAddressModeW(vk::SamplerAddressMode::eClampToBorder)
		.setAnisotropyEnable(vk::True)
		.setMaxAnisotropy(maxAnisotropy)
		.setBorderColor(vk::BorderColor::eFloatOpaqueWhite)
		.setUnnormalizedCoordinates(vk::False)
		.setCompareEnable(vk::True) //hardware depth comparison
		.setCompareOp(vk::CompareOp::eLessOrEqual)
		.setMipmapMode(vk::SamplerMipmapMode::eNearest)
		.setMipLodBias(0.0f)
		.setMinLod(0.0f)
		.setMaxLod(1.0f);
		
		samplers[static_cast<uint32_t>(SamplerType::DEPTH)] = device.createSampler(samplerInfo);


		

	

	vk::DescriptorImageInfo txtSamplerI;
		txtSamplerI
			.setSampler(samplers[static_cast<uint32_t>(SamplerType::TEXTURE)]);
	vk::DescriptorImageInfo depthSamplerI{};
		depthSamplerI
			.setSampler(samplers[static_cast<uint32_t>(SamplerType::DEPTH)]);
	std::array<vk::DescriptorImageInfo,2> smpInfo = {txtSamplerI,depthSamplerI};

	vk::WriteDescriptorSet writeInfo;
	writeInfo
		.setDstBinding(0)
		.setDstSet(descriptorSets[static_cast<size_t>(DescriptorSetType::SAMPLER)])
		.setDescriptorType(vk::DescriptorType::eSampler)
		.setPImageInfo(smpInfo.data())
		.setDstArrayElement(0)
		.setDescriptorCount(2);
	
	device.updateDescriptorSets(writeInfo, {});
}

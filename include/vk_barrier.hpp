#pragma once
#include "shared_definitions.hpp"
#include "vulkan/vulkan.hpp"
    
namespace vkutils{

void setPipelineBarrier(vk::CommandBuffer cmdBuffer, vk::Image image,  vk::ImageLayout oldLayout, vk::ImageLayout newLayout,vk::ImageAspectFlagBits aspectImage, BarrierMasks mask = {});


void setPipelineBarrier(vk::CommandBuffer cmdBuffer, vk::Buffer buffer,vk::DeviceSize byteSize, BarrierMasks mask = {});

};
#pragma once
#include "vulkan/vulkan.hpp"
    struct BarrierMasks {
    vk::PipelineStageFlags2 srcStage{};
    vk::PipelineStageFlags2 dstStage{};
    vk::AccessFlags2 srcAccess{};
    vk::AccessFlags2 dstAccess{};

    bool notFilled() const {
        return srcStage == vk::PipelineStageFlags2{} &&
            dstStage == vk::PipelineStageFlags2{} &&
            srcAccess == vk::AccessFlags2{} &&
            dstAccess == vk::AccessFlags2{};
    }
};

struct PipelineBundle{
    vk::Pipeline handle;
    vk::PipelineLayout layout{};
};

struct CreatePipelineInfo {
    int clrAttachCount{};
    vk::Format clrFormat{};
    vk::Format depthFormat{};
    bool isDepthPass{};
    vk::PrimitiveTopology top{};
    vk::Extent2D viewportExtent{};
    vk::PolygonMode polyMode{};
    vk::FrontFace fFace{};
    size_t bytesPC{};
    vk::ShaderStageFlags stagePC{};
    vk::Bool32 depthTestEnable{};
    vk::Bool32 depthWriteEnable{};
    std::vector<vk::DescriptorSetLayout> descSetLayouts{};
    // Fluent Setters
    CreatePipelineInfo& setClrAttachCount(uint32_t count) { clrAttachCount = count; return *this; }
    CreatePipelineInfo& setClrFormat(vk::Format format) { clrFormat = format; return *this; }
    CreatePipelineInfo& setDepthFormat(vk::Format format) { depthFormat = format; return *this; }
    CreatePipelineInfo& setIsDepthPass(bool depthPass) { isDepthPass = depthPass; return *this; }
    CreatePipelineInfo& setTopology(vk::PrimitiveTopology t) { top = t; return *this; }
    CreatePipelineInfo& setExtent(vk::Extent2D extent) { viewportExtent = extent; return *this; }
    CreatePipelineInfo& setPolyMode(vk::PolygonMode mode) { polyMode = mode; return *this; }
    CreatePipelineInfo& setFrontFace(vk::FrontFace face) { fFace = face; return *this; }
    CreatePipelineInfo& setBytesPC(uint32_t bytes) { bytesPC = bytes; return *this; }
    CreatePipelineInfo& setStagePC(vk::ShaderStageFlags flags) { stagePC = flags; return *this; }
    CreatePipelineInfo& setDepthTest(vk::Bool32 enable) { depthTestEnable = enable; return *this; }
    CreatePipelineInfo& setDepthWrite(vk::Bool32 enable) { depthWriteEnable = enable; return *this; }
    CreatePipelineInfo& setDescSetLayout(std::vector<vk::DescriptorSetLayout> layouts) { descSetLayouts = layouts; return *this; }

    
};
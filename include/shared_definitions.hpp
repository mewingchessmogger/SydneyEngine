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


#pragma once
#include "vulkan/vulkan.hpp"
class Renderer {
    public:
    void createSwapchain();
    void clearSwapchain();
    void rethinkSwapchain();
    bool isValidSwapchain(vk::ResultValue<uint32_t> imgResult, vk::Semaphore imageReadySemaphore);
};
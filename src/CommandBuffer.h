#pragma once
#include <vulkan/vulkan.h>
#include "Device.h"
#include "GraphicsPipeline.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

class CommandBuffer{
    public:
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        CommandBuffer(Device* device, Swapchain* swapchain, GraphicsPipeline* graphicsPipeline);
        ~CommandBuffer();

        void waitForFences();
        void resetFences();

        void acquireNextImage();

        void submitCommandBuffer();
        
        bool framebufferResized = false;
    private:
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences; 
        
        uint32_t imageIndex;
        uint32_t currentFrame = 0;
        
        Swapchain* swapchain;
        GraphicsPipeline* graphicsPipeline;
        Device* device;
};
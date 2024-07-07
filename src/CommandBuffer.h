#pragma once
#include <vulkan/vulkan.h>
#include "Device.h"
#include "GraphicsPipeline.h"

class CommandBuffer{
    public:
        void createCommandPool();
        void createCommandBuffer();
        void createSyncObjects();
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        CommandBuffer(Device* device, Swapchain* swapchain, GraphicsPipeline* graphicsPipeline);
        ~CommandBuffer();

        void waitForAndResetFences();

        void acquireNextImage();

        void submitCommandBuffer();
        

    private:
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence; 
        
        uint32_t imageIndex;
        
        Swapchain* swapchain;
        GraphicsPipeline* graphicsPipeline;
        Device* device;
};
#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>
#include "Device.h"
#include "Swapchain.h"
class GraphicsPipeline{
    public:
        GraphicsPipeline(Device* newDevice,Swapchain* swapchain);
        ~GraphicsPipeline();
        static std::vector<char> readFile(const std::string& filename);
        VkShaderModule createShaderModule(const std::vector<char>& code);
        void createRenderPass();
        void createFramebuffers();
        void cleanupFramebuffers(){for (auto framebuffer : swapChainFramebuffers){
            vkDestroyFramebuffer(device->device(),framebuffer,nullptr);
        }}

        void recreateSwapChain(){
            vkDeviceWaitIdle(device->device());
            cleanupFramebuffers();
            swapchain->cleanupSwapChain();

            swapchain->createSwapChain();
            createFramebuffers();
        }
    
        std::vector<VkFramebuffer> get_swapChainFramebuffers(){return swapChainFramebuffers;} 
        VkRenderPass get_renderPass(){return renderPass;}
        VkPipeline get_graphicsPipeline(){return graphicsPipeline;}
    private:
        Device* device;
        Swapchain* swapchain;
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;


        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkPipeline graphicsPipeline;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
};
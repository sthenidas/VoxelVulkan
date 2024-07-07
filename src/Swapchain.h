#pragma once
#include "Device.h"
#include <limits>
#include <algorithm>

class Swapchain{
    public: 
        Swapchain(Device* device, Window* window );
        ~Swapchain();
        void createSwapChain();
        
        void cleanupSwapChain();
       
        
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void createImageViews();
        VkExtent2D get_swapChainExtent(){return swapChainExtent;}
        VkFormat get_swapChainImageFormat(){return swapChainImageFormat;}
        std::vector<VkImageView> get_swapChainImageViews(){return swapChainImageViews;}

        VkSwapchainKHR get_swapChain(){return swapChain;}
    private:
        Device* device;
        VkSwapchainKHR swapChain;
        VkSurfaceKHR surface;
        GLFWwindow* window;
        uint32_t imageCount;

       

        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

};
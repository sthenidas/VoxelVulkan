#pragma once

#include "Instance.h"
#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <vulkan/vulkan_beta.h>
#include "Window.h"

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete(){
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};




class Device{
    public:
        Device(Instance* instance, Window* window); // analogous to the pickPhysicalDevice() in the tutorial
        ~Device();
        

        QueueFamilyIndices findQueueFamilies();

        void createLogicalDevice();
    
        SwapChainSupportDetails querySwapChainSupport();

        VkDevice device(){return logicalDevice;}

        VkQueue get_graphicsQueue(){return graphicsQueue;}

        VkQueue get_presentQueue(){return presentQueue;}
        
    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice logicalDevice;
        VkQueue graphicsQueue;
        VkSurfaceKHR surface;
        VkQueue presentQueue;
        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        // Device object will be implicitly destroyed when VkInstance is destroyed, so it won't outlive this
        // pointer.
        

        bool isDeviceSuitable(VkPhysicalDevice device); //Checks passed device is suitable for some purpose.
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};


#pragma once
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>


const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const bool enableValidationLayers = true;

class Instance{
    public:
        Instance();
        ~Instance();
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();

        VkResult CreateDebugUtilsMessengerEXT( const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator);
        void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);
        void setupDebugMessenger();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
            
        std::cerr << "Validation Layer:" << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    
        }
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkInstance get_VkInstance();
    private:
        VkInstance instance;
        
        VkDebugUtilsMessengerEXT debugMessenger;
        std::vector<const char*> requiredExtensions;
        
};
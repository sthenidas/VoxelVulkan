
#include "Instance.h"

Instance::Instance(){
     
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Requested validation layers unavailable.");
        }

        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Vulkan",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Pulsar",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0
        };
        auto extensions = getRequiredExtensions();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        
        populateDebugMessengerCreateInfo(debugCreateInfo);

        VkInstanceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = (enableValidationLayers) ? static_cast<uint32_t>(validationLayers.size()) : 0,
            .ppEnabledLayerNames = (enableValidationLayers) ? validationLayers.data() : nullptr,
            .pNext = (enableValidationLayers) ? (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo : nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
        };

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance.");
        }
    
}

Instance::~Instance(){
    if (enableValidationLayers){
        DestroyDebugUtilsMessengerEXT(nullptr);
    }
    vkDestroyInstance(instance,nullptr);
    

}

bool Instance::checkValidationLayerSupport(){

   
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if(strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    
}

std::vector<const char*> Instance::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            
            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if(enableValidationLayers)
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            #if __APPLE__
                extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            #endif
            
            return extensions;
}


void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
    createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = 
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = 
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debugCallback
            };
}

void Instance::setupDebugMessenger(){
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr) != VK_SUCCESS) 
        { 
            throw std::runtime_error("Failed to set up debug messenger.");
        }
}

VkResult Instance::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"); 

    if (func != nullptr) 
    {
        return func(instance, pCreateInfo, pAllocator, &debugMessenger);
    } else 
    { 
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Instance::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"); 
    
    if (func != nullptr) 
    {
        func(instance, debugMessenger, pAllocator);
    } 
}

VkInstance Instance::get_VkInstance(){
    return instance;
}
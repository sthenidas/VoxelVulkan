#include "Device.h"

Device::Device(Instance* instance, Window* window){
    
    surface = window->get_surface();
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->get_VkInstance(),&deviceCount,nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance->get_VkInstance(),&deviceCount,devices.data());


    // Optionally, rank the devices and use the best. 
    //(This is found in the physical devices seciton, not implemented)
    for (const auto& device: devices){
        if (isDeviceSuitable(device)){
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    createLogicalDevice();
    QueueFamilyIndices indices = findQueueFamilies();
    vkGetDeviceQueue(logicalDevice,indices.graphicsFamily.value(),0,&graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

Device::~Device(){
    vkDestroyDevice(logicalDevice,nullptr);
}

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device){
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface, &formatCount, nullptr);

    if (formatCount != 0){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface, &formatCount,details.formats.data());
    }


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface, &presentModeCount, nullptr);

    if (presentModeCount != 0){
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface, &presentModeCount,details.presentModes.data());
    }

    return details;
}

SwapChainSupportDetails Device::querySwapChainSupport(){
    VkPhysicalDevice device = physicalDevice;
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface, &formatCount, nullptr);

    if (formatCount != 0){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface, &formatCount,details.formats.data());
    }


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface, &presentModeCount, nullptr);

    if (presentModeCount != 0){
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface, &presentModeCount,details.presentModes.data());
    }
    return details;
}

bool Device::isDeviceSuitable(VkPhysicalDevice device){

    // Checks that the device has a queue family that supports (in the case of the tutorial,
    // the VK_QUEUE_GRAPHICS_BIT. Might be worth printing all of the features of the queues tho.)
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported){
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
    
    // Below is a more complicated example of how one might filter for specific GPU features.

    /*
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
        deviceFeatures.geometryShader;
        */
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);

    vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,availableExtensions.data());
    std::set<std::string> requiredExtensions(deviceExtensions.begin(),deviceExtensions.end());

    for (const auto& extension : availableExtensions){
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

QueueFamilyIndices Device::findQueueFamilies(){
    VkPhysicalDevice device = physicalDevice;
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int i = 0;
    for (const auto& queueFamily : queueFamilies){

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        if (indices.isComplete()){
            break;
        }
        i++;
    }

    return indices;
}

void Device::createLogicalDevice(){
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamiles = {indices.graphicsFamily.value(), indices.presentFamily.value()};



    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamiles){        
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures deviceFeatures{}; //Don't need anything special right now.
    deviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);


    // Layers are ignored here in up-to-date implementations, not going to include them.
    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queueCreateInfos.data(),
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
    };

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS){
        throw std::runtime_error("failed to create logical device");
    }
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int i = 0;
    for (const auto& queueFamily : queueFamilies){

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        if (indices.isComplete()){
            break;
        }
        i++;
    }

    return indices;
}
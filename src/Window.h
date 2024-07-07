#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Instance.h"
class Window{
    public:
        Window();
        ~Window();
        bool shouldClose();
        void pollEvents();
        void createSurface(Instance* instance);
        VkSurfaceKHR get_surface(){return surface;}
        GLFWwindow* get_pWindow(){return window;}
    private:
        GLFWwindow* window;
        VkSurfaceKHR surface;
        Instance* instance;

        const uint32_t WIDTH = 800; 
        const uint32_t HEIGHT = 600;

};
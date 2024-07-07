#include "Window.h"

Window::Window(){
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

Window::~Window(){
    vkDestroySurfaceKHR(instance->get_VkInstance(),surface,nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::shouldClose(){
    return glfwWindowShouldClose(window);
}

void Window::pollEvents(){
    glfwPollEvents();
}

void Window::createSurface(Instance* newInstance){
    instance = newInstance;

    if (glfwCreateWindowSurface(instance->get_VkInstance(), window, nullptr, &surface)){
            throw std::runtime_error("failed to create window surface!");
        }
}

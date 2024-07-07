// Vulkan API included through GLFW window management.

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>


#include "Instance.h"
#include "Device.h"
#include "Swapchain.h"
#include "Window.h"
#include "GraphicsPipeline.h"
#include "CommandBuffer.h"





class Application 
{
public:

    void run() 
    {
        //initWindow();
        window = new Window();
        glfwSetWindowUserPointer(window->get_pWindow(), this);
        glfwSetFramebufferSizeCallback(window->get_pWindow(),framebufferResizeCallback);
        
        initVulkan();
        mainLoop();
        cleanup();
    }

    void set_framebufferResized(bool newBool){
        commandBuffer->framebufferResized = newBool;
    }

private:   

    // ---------- OVERARCHING ---------- //

    

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height){
        auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->set_framebufferResized(true);
    }

    void initVulkan()
    {
        instance = new Instance();
        instance->setupDebugMessenger();
        window->createSurface(instance);
        

        

        device = new Device(instance,window);
        swapchain = new Swapchain(device, window);
        graphicsPipeline = new GraphicsPipeline(device,swapchain);
        commandBuffer = new CommandBuffer(device,swapchain,graphicsPipeline);
        

        
    }

    void mainLoop()
    {
        while (!window->shouldClose())
        {
            window->pollEvents();
            drawFrame();

        }
    }

    void drawFrame(){
        
        int width = 0, height = 0;
        glfwGetFramebufferSize(window->get_pWindow(),&width,&height);
        while (width == 0 || height == 0){
            glfwGetFramebufferSize(window->get_pWindow(),&width,&height);
            glfwWaitEvents();
        }
        commandBuffer->waitForFences();
        commandBuffer->acquireNextImage();
        commandBuffer->resetFences();
        commandBuffer->submitCommandBuffer();
    }
    void cleanup()
    {   
        // the order to these deletions is important, as it basically represents the hierarchy going on.
        // notice it's basically the reverse of the initVulkan()
        
        delete commandBuffer;
        delete graphicsPipeline;
        delete swapchain;
        delete device;

        delete window;
        delete instance;

        
    }

   
    
    // ---------- HELPER ---------- //

private:

    Window* window;

    
    Instance* instance;

    Device* device;
    Swapchain* swapchain;
    GraphicsPipeline* graphicsPipeline;
    CommandBuffer* commandBuffer;

};

int main() 
{
    Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
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
        initVulkan();
        mainLoop();
        cleanup();
    }

private:   

    // ---------- OVERARCHING ---------- //

    

    

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
        
        commandBuffer->waitForAndResetFences();
        commandBuffer->acquireNextImage();
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
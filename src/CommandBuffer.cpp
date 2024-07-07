#include "CommandBuffer.h"
#include <iostream>
CommandBuffer::CommandBuffer(Device* device, Swapchain* swapchain, GraphicsPipeline* graphicsPipeline){
    this->device = device;
    this->swapchain = swapchain;
    this->graphicsPipeline = graphicsPipeline;
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
    
}

CommandBuffer::~CommandBuffer(){

    
    //vkWaitForFences(device->device(),1,&inFlightFence,VK_TRUE,UINT64_MAX);
    vkDeviceWaitIdle(device->device()); //waits for everything to calm down before running destroyers
    vkDestroySemaphore(device->device(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device->device(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(device->device(), inFlightFence, nullptr);
    vkDestroyCommandPool(device->device(),commandPool,nullptr);
    
}
void CommandBuffer::createCommandPool(){
    QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies();

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
    };

    if (vkCreateCommandPool(device->device(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS){
        throw std::runtime_error("failed to create command pool!");
    }

}

void CommandBuffer::createCommandBuffer(){
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    if (vkAllocateCommandBuffers(device->device(), &allocInfo, &commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("failed to allocate command buffers!");
    }


    
}

void CommandBuffer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageindex){
    
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS){
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    auto swapChainFramebuffers = graphicsPipeline->get_swapChainFramebuffers();
    
    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = graphicsPipeline->get_renderPass(),
        .framebuffer = swapChainFramebuffers[imageindex],
        .renderArea.offset = {0,0},
        .renderArea.extent = swapchain->get_swapChainExtent(),
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline->get_graphicsPipeline());
    

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapchain->get_swapChainExtent().width),
        .height = static_cast<float>(swapchain->get_swapChainExtent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
        

       
    };    

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    VkRect2D scissor = {
        .offset = {0,0},
        .extent = swapchain->get_swapChainExtent(),
    };
    
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer)!= VK_SUCCESS){
        throw std::runtime_error("failed to record command buffer!");
    }

}

void CommandBuffer::acquireNextImage(){
            
    vkAcquireNextImageKHR(device->device(),swapchain->get_swapChain(),
        UINT64_MAX,imageAvailableSemaphore,VK_NULL_HANDLE,&imageIndex);
    
    vkResetCommandBuffer(commandBuffer,0);
    
    recordCommandBuffer(commandBuffer,imageIndex);
    
    
}

void CommandBuffer::waitForAndResetFences(){
    vkWaitForFences(device->device(), 1, &inFlightFence,VK_TRUE,UINT64_MAX);
    vkResetFences(device->device(),1,&inFlightFence);
}

void CommandBuffer::createSyncObjects(){
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    if (vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device->device(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS){
            throw std::runtime_error("failed to create semaphores and fence!");
        }
}

void CommandBuffer::submitCommandBuffer(){
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    if (vkQueueSubmit(device->get_graphicsQueue(),1,&submitInfo,inFlightFence) != VK_SUCCESS){
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChain[] = {swapchain->get_swapChain()};
    
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChain,
        .pImageIndices = &imageIndex,
        .pResults = nullptr
    };

    vkQueuePresentKHR(device->get_presentQueue(), &presentInfo);

}


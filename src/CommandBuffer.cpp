#include "CommandBuffer.h"
#include <iostream>
CommandBuffer::CommandBuffer(Device* device, Swapchain* swapchain, GraphicsPipeline* graphicsPipeline){
    this->device = device;
    this->swapchain = swapchain;
    this->graphicsPipeline = graphicsPipeline;
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
    
}

CommandBuffer::~CommandBuffer(){

    
    //vkWaitForFences(device->device(),1,&inFlightFence,VK_TRUE,UINT64_MAX);
    vkDeviceWaitIdle(device->device()); //waits for everything to calm down before running destroyers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(device->device(), imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device->device(), renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device->device(), inFlightFences[i], nullptr);
    }
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

void CommandBuffer::createCommandBuffers(){

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) commandBuffers.size(),
    };

    if (vkAllocateCommandBuffers(device->device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
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
            
    VkResult result = vkAcquireNextImageKHR(device->device(),swapchain->get_swapChain(),
        UINT64_MAX,imageAvailableSemaphores[currentFrame],VK_NULL_HANDLE,&imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR){
        graphicsPipeline->recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetCommandBuffer(commandBuffers[currentFrame],0);
    
    recordCommandBuffer(commandBuffers[currentFrame],imageIndex);
    
    
}

void CommandBuffer::waitForFences(){
    vkWaitForFences(device->device(), 1, &inFlightFences[currentFrame],VK_TRUE,UINT64_MAX);
}

void CommandBuffer::resetFences(){
    vkResetFences(device->device(),1,&inFlightFences[currentFrame]);
}

void CommandBuffer::createSyncObjects(){

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
    if (vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device->device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create semaphores and fence!");
        }
    }
}

void CommandBuffer::submitCommandBuffer(){
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffers[currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    if (vkQueueSubmit(device->get_graphicsQueue(),1,&submitInfo,inFlightFences[currentFrame]) != VK_SUCCESS){
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

    VkResult result = vkQueuePresentKHR(device->get_presentQueue(), &presentInfo);
    

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        graphicsPipeline->recreateSwapChain();
        framebufferResized = false;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


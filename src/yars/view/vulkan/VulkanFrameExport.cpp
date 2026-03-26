// Frame export implementation for VulkanContext
// Captures rendered frames and exports them to PNG files

#include "VulkanContext.h"
#include "VulkanBuffers.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cstring>

namespace yars {

bool VulkanContext::exportFrame(const std::string& filename) {
    std::cout << "VulkanContext: Exporting frame to " << filename << std::endl;

    // Wait for rendering to complete
    vkQueueWaitIdle(_graphicsQueue);

    // Get the current swapchain image
    uint32_t imageIndex = (_currentFrame + MAX_FRAMES_IN_FLIGHT - 1) % MAX_FRAMES_IN_FLIGHT;
    VkImage srcImage = _swapchainImages[imageIndex];

    // Image dimensions
    uint32_t width = _swapchainExtent.width;
    uint32_t height = _swapchainExtent.height;

    // Create staging buffer to copy image data
    VkDeviceSize imageSize = width * height * 4; // RGBA
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VulkanBuffer bufferHelper(this);
    if (!bufferHelper.createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory)) {
        std::cerr << "Failed to create staging buffer for frame export" << std::endl;
        return false;
    }

    // Create command buffer for image copy
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // Transition swapchain image to transfer source layout
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = srcImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    // Copy image to buffer
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyImageToBuffer(
        commandBuffer,
        srcImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        stagingBuffer,
        1,
        &region
    );

    // Transition swapchain image back to present layout
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkEndCommandBuffer(commandBuffer);

    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_graphicsQueue);

    // Map staging buffer memory
    void* data;
    vkMapMemory(_device, stagingBufferMemory, 0, imageSize, 0, &data);

    // Convert BGRA to RGBA and flip Y axis
    std::vector<unsigned char> pixels(imageSize);
    unsigned char* src = static_cast<unsigned char*>(data);

    // Check swapchain format to determine if conversion is needed
    bool needsConversion = (_swapchainImageFormat == VK_FORMAT_B8G8R8A8_UNORM ||
                           _swapchainImageFormat == VK_FORMAT_B8G8R8A8_SRGB);

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint32_t srcIdx = (y * width + x) * 4;
            uint32_t dstIdx = ((height - 1 - y) * width + x) * 4;  // Flip Y

            if (needsConversion) {
                // BGRA to RGBA
                pixels[dstIdx + 0] = src[srcIdx + 2]; // R
                pixels[dstIdx + 1] = src[srcIdx + 1]; // G
                pixels[dstIdx + 2] = src[srcIdx + 0]; // B
                pixels[dstIdx + 3] = src[srcIdx + 3]; // A
            } else {
                // Direct copy
                pixels[dstIdx + 0] = src[srcIdx + 0];
                pixels[dstIdx + 1] = src[srcIdx + 1];
                pixels[dstIdx + 2] = src[srcIdx + 2];
                pixels[dstIdx + 3] = src[srcIdx + 3];
            }
        }
    }

    vkUnmapMemory(_device, stagingBufferMemory);

    // Write PNG file
    int result = stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4);

    // Cleanup
    vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
    vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingBufferMemory, nullptr);

    if (result == 0) {
        std::cerr << "Failed to write PNG file: " << filename << std::endl;
        return false;
    }

    std::cout << "Frame exported successfully to " << filename << std::endl;
    return true;
}

} // namespace yars

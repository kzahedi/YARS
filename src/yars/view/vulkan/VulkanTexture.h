#ifndef __VULKAN_TEXTURE_H__
#define __VULKAN_TEXTURE_H__

#include <vulkan/vulkan.h>
#include <string>

namespace yars {

class VulkanContext;

/**
 * VulkanTexture - Manages a single texture image for Vulkan rendering
 *
 * Responsibilities:
 * - Load image from file (PNG/JPEG via stb_image)
 * - Create VkImage for GPU storage
 * - Create VkImageView for shader access
 * - Create VkSampler for texture filtering
 * - Manage descriptor set for texture binding
 *
 * Usage:
 *   VulkanTexture* tex = new VulkanTexture(context);
 *   if (tex->loadFromFile("wood2.jpg")) {
 *       // Bind texture to descriptor set
 *       tex->bind(descriptorSet, binding);
 *   }
 */
class VulkanTexture {
public:
    VulkanTexture(VulkanContext* context);
    ~VulkanTexture();

    /**
     * Load texture from file (PNG, JPEG, BMP, TGA, etc.)
     * Uses stb_image for decoding
     *
     * @param filename Path to image file (relative or absolute)
     * @return true if successfully loaded and uploaded to GPU
     */
    bool loadFromFile(const std::string& filename);

    /**
     * Create texture from raw RGBA data
     *
     * @param pixels RGBA pixel data (4 bytes per pixel)
     * @param width Image width in pixels
     * @param height Image height in pixels
     * @return true if successfully created
     */
    bool createFromData(const unsigned char* pixels, uint32_t width, uint32_t height);

    /**
     * Get the image view for shader access
     */
    VkImageView getImageView() const { return _imageView; }

    /**
     * Get the sampler for texture filtering
     */
    VkSampler getSampler() const { return _sampler; }

    /**
     * Get image dimensions
     */
    uint32_t getWidth() const { return _width; }
    uint32_t getHeight() const { return _height; }

    /**
     * Check if texture is valid and ready for use
     */
    bool isValid() const { return _image != VK_NULL_HANDLE; }

    /**
     * Cleanup all Vulkan resources
     */
    void cleanup();

private:
    /**
     * Create VkImage and allocate device memory
     */
    bool createImage(uint32_t width, uint32_t height, VkFormat format);

    /**
     * Create VkImageView for shader access
     */
    bool createImageView(VkFormat format);

    /**
     * Create VkSampler for texture filtering
     */
    bool createSampler();

    /**
     * Transition image layout (undefined → transfer dst → shader read)
     */
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

    /**
     * Copy pixel data from staging buffer to image
     */
    void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);

    /**
     * Find suitable memory type for allocation
     */
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VulkanContext* _context;

    VkImage _image;
    VkDeviceMemory _imageMemory;
    VkImageView _imageView;
    VkSampler _sampler;

    uint32_t _width;
    uint32_t _height;
    VkFormat _format;
};

} // namespace yars

#endif // __VULKAN_TEXTURE_H__

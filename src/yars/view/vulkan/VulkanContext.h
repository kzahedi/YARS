#ifndef __VULKAN_CONTEXT_H__
#define __VULKAN_CONTEXT_H__

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <optional>

namespace yars {

// Forward declarations
class VulkanSceneGraph;
class VulkanSceneNode;
class VulkanTexture;

/**
 * Core Vulkan context - manages instance, device, queues, swapchain
 * Singleton pattern for global access
 */
class VulkanContext {
public:
    static VulkanContext* instance();
    static void destroy();

    // Initialization
    bool initialize(SDL_Window* window);
    void cleanup();

    // Getters
    VkInstance getInstance() const { return _instance; }
    VkPhysicalDevice getPhysicalDevice() const { return _physicalDevice; }
    VkDevice getDevice() const { return _device; }
    VkQueue getGraphicsQueue() const { return _graphicsQueue; }
    VkQueue getPresentQueue() const { return _presentQueue; }
    VkSurfaceKHR getSurface() const { return _surface; }
    VkSwapchainKHR getSwapchain() const { return _swapchain; }
    VkFormat getSwapchainImageFormat() const { return _swapchainImageFormat; }
    VkExtent2D getSwapchainExtent() const { return _swapchainExtent; }
    const std::vector<VkImageView>& getSwapchainImageViews() const { return _swapchainImageViews; }
    VkCommandPool getCommandPool() const { return _commandPool; }
    uint32_t getCurrentFrame() const { return _currentFrame; }

    // Queue family indices
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    QueueFamilyIndices getQueueFamilies() const { return _queueFamilyIndices; }

    // Swapchain support details
    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device) const;

    // Swapchain recreation (for window resize)
    void recreateSwapchain();

    // Rendering
    void drawFrame(VulkanSceneGraph* sceneGraph = nullptr);

    // Frame export
    bool exportFrame(const std::string& filename);

    // Uniform buffer updates
    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& model,
                            const glm::mat4& view, const glm::mat4& proj);

    // Geometry binding
    void setActiveGeometry(VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t indexCount);

    // Texture binding
    void bindTexture(VkImageView imageView, VkSampler sampler);

    // Descriptor set management for nodes
    bool allocateNodeDescriptorSets(VulkanTexture* texture, std::vector<VkDescriptorSet>& descriptorSets);

    // Camera controls
    void handleMouseMotion(int dx, int dy, bool leftButton, bool rightButton);
    void handleKeyboard(const bool* keys, float deltaTime);
    void resetCamera();
    void setCameraPosition(const glm::vec3& position, const glm::vec3& target);
    void setCameraFromYARS(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& up);

    // Camera getters for testing/verification
    glm::vec3 getCameraPosition() const { return _cameraPosition; }
    glm::vec3 getCameraTarget() const { return _cameraTarget; }
    glm::vec3 getCameraUp() const { return _cameraUp; }

private:
    // Helper to render individual scene node
    void renderSceneNode(VulkanSceneNode* node, VkCommandBuffer cmd, uint32_t frameIndex,
                        const glm::mat4& view, const glm::mat4& proj);
    VulkanContext() = default;
    ~VulkanContext() = default;
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    static VulkanContext* _singleton;

    // Initialization helpers
    bool createInstance();
    bool setupDebugMessenger();
    bool createSurface(SDL_Window* window);
    bool pickPhysicalDevice();
    bool createLogicalDevice();
    bool createSwapchain();
    bool createImageViews();
    bool createRenderPass();
    bool createDepthResources();
    bool createGraphicsPipeline();
    bool createFramebuffers();
    bool createCommandPool();
    bool createCommandBuffers();
    bool createSyncObjects();
    bool createDescriptorSetLayout();
    bool createUniformBuffers();
    bool createDescriptorPool();
    bool createDescriptorSets();

    void cleanupSwapchain();

    // Shader loading
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    // Helper functions
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    // Vulkan objects
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    VkQueue _graphicsQueue = VK_NULL_HANDLE;
    VkQueue _presentQueue = VK_NULL_HANDLE;

    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> _swapchainImages;
    VkFormat _swapchainImageFormat;
    VkExtent2D _swapchainExtent;
    std::vector<VkImageView> _swapchainImageViews;

    QueueFamilyIndices _queueFamilyIndices;
    SDL_Window* _window = nullptr;

    // Depth buffer resources
    VkImage _depthImage = VK_NULL_HANDLE;
    VkDeviceMemory _depthImageMemory = VK_NULL_HANDLE;
    VkImageView _depthImageView = VK_NULL_HANDLE;
    VkFormat _depthFormat;

    // Rendering pipeline
    VkRenderPass _renderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> _swapchainFramebuffers;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    VkPipeline _graphicsPipeline = VK_NULL_HANDLE;

    // Command pool and buffers
    VkCommandPool _commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> _commandBuffers;

    // Descriptor sets for uniforms
    VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> _descriptorSets;

    // Uniform buffers (one per frame in flight)
    std::vector<VkBuffer> _uniformBuffers;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;
    std::vector<void*> _uniformBuffersMapped;

    // Synchronization
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    uint32_t _currentFrame = 0;
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    // Active geometry for rendering
    VkBuffer _activeVertexBuffer = VK_NULL_HANDLE;
    VkBuffer _activeIndexBuffer = VK_NULL_HANDLE;
    uint32_t _activeIndexCount = 0;

        // Camera state
    // Oblique view to see walls from an angle (not straight top-down)
    // Position camera at angle to see both horizontal layout and vertical walls
    // Camera at (8, 8, 8) gives nice 45-degree oblique view of the arena
    glm::vec3 _cameraPosition = glm::vec3(8.0f, 8.0f, 8.0f);
    glm::vec3 _cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float _cameraDistance = 10.0f;
    float _cameraYaw = 0.0f;
    float _cameraPitch = -45.0f;    // 45-degree angle from horizontal
    float _cameraSpeed = 5.0f;      // Movement speed
    float _mouseSensitivity = 0.2f; // Mouse rotation sensitivity

    // Validation layers
    const std::vector<const char*> _validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> _deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_portability_subset"  // Required for MoltenVK on macOS
    };

#ifdef NDEBUG
    bool _enableValidationLayers = false;
#else
    bool _enableValidationLayers = true;
#endif
};

} // namespace yars

#endif // __VULKAN_CONTEXT_H__

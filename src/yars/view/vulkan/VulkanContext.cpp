#include "VulkanContext.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <cstring>

namespace yars {

VulkanContext* VulkanContext::_singleton = nullptr;

VulkanContext* VulkanContext::instance() {
    if (!_singleton) {
        _singleton = new VulkanContext();
    }
    return _singleton;
}

void VulkanContext::destroy() {
    if (_singleton) {
        _singleton->cleanup();
        delete _singleton;
        _singleton = nullptr;
    }
}

bool VulkanContext::initialize(SDL_Window* window) {
    _window = window;

    std::cout << "Initializing Vulkan..." << std::endl;

    if (!createInstance()) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return false;
    }

    std::cout << "Setting up debug messenger..." << std::endl;
    if (!setupDebugMessenger()) {
        std::cerr << "Failed to setup debug messenger!" << std::endl;
        return false;
    }

    std::cout << "Creating window surface..." << std::endl;
    if (!createSurface(window)) {
        std::cerr << "Failed to create window surface!" << std::endl;
        return false;
    }

    std::cout << "Selecting GPU..." << std::endl;
    if (!pickPhysicalDevice()) {
        std::cerr << "Failed to find a suitable GPU!" << std::endl;
        return false;
    }

    std::cout << "Creating logical device..." << std::endl;
    if (!createLogicalDevice()) {
        std::cerr << "Failed to create logical device!" << std::endl;
        return false;
    }

    std::cout << "Creating swapchain..." << std::endl;
    if (!createSwapchain()) {
        std::cerr << "Failed to create swapchain!" << std::endl;
        return false;
    }

    std::cout << "Creating image views..." << std::endl;
    if (!createImageViews()) {
        std::cerr << "Failed to create image views!" << std::endl;
        return false;
    }

    std::cout << "Creating depth resources..." << std::endl;
    if (!createDepthResources()) {
        std::cerr << "Failed to create depth resources!" << std::endl;
        return false;
    }

    std::cout << "Creating render pass..." << std::endl;
    if (!createRenderPass()) {
        std::cerr << "Failed to create render pass!" << std::endl;
        return false;
    }

    std::cout << "Creating descriptor set layout..." << std::endl;
    if (!createDescriptorSetLayout()) {
        std::cerr << "Failed to create descriptor set layout!" << std::endl;
        return false;
    }

    std::cout << "Creating graphics pipeline..." << std::endl;
    if (!createGraphicsPipeline()) {
        std::cerr << "Failed to create graphics pipeline!" << std::endl;
        return false;
    }

    std::cout << "Creating framebuffers..." << std::endl;
    if (!createFramebuffers()) {
        std::cerr << "Failed to create framebuffers!" << std::endl;
        return false;
    }

    std::cout << "Creating command pool..." << std::endl;
    if (!createCommandPool()) {
        std::cerr << "Failed to create command pool!" << std::endl;
        return false;
    }

    std::cout << "Creating uniform buffers..." << std::endl;
    if (!createUniformBuffers()) {
        std::cerr << "Failed to create uniform buffers!" << std::endl;
        return false;
    }

    std::cout << "Creating descriptor pool..." << std::endl;
    if (!createDescriptorPool()) {
        std::cerr << "Failed to create descriptor pool!" << std::endl;
        return false;
    }

    std::cout << "Creating descriptor sets..." << std::endl;
    if (!createDescriptorSets()) {
        std::cerr << "Failed to create descriptor sets!" << std::endl;
        return false;
    }

    std::cout << "Creating command buffers..." << std::endl;
    if (!createCommandBuffers()) {
        std::cerr << "Failed to create command buffers!" << std::endl;
        return false;
    }

    std::cout << "Creating synchronization objects..." << std::endl;
    if (!createSyncObjects()) {
        std::cerr << "Failed to create sync objects!" << std::endl;
        return false;
    }

    std::cout << "Vulkan initialized successfully!" << std::endl;
    return true;
}

void VulkanContext::cleanup() {
    if (_device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(_device);
    }

    // Clean up synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (_renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
        }
        if (_imageAvailableSemaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
        }
        if (_inFlightFences[i] != VK_NULL_HANDLE) {
            vkDestroyFence(_device, _inFlightFences[i], nullptr);
        }
    }

    // Clean up uniform buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (_uniformBuffers[i] != VK_NULL_HANDLE) {
            vkDestroyBuffer(_device, _uniformBuffers[i], nullptr);
        }
        if (_uniformBuffersMemory[i] != VK_NULL_HANDLE) {
            vkFreeMemory(_device, _uniformBuffersMemory[i], nullptr);
        }
    }

    // Clean up descriptor pool (this also frees descriptor sets)
    if (_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        _descriptorPool = VK_NULL_HANDLE;
    }

    // Clean up descriptor set layout
    if (_descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
        _descriptorSetLayout = VK_NULL_HANDLE;
    }

    // Clean up command pool (this also frees command buffers)
    if (_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }

    // Clean up pipeline and layout
    if (_graphicsPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
        _graphicsPipeline = VK_NULL_HANDLE;
    }

    if (_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        _pipelineLayout = VK_NULL_HANDLE;
    }

    // Clean up render pass
    if (_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(_device, _renderPass, nullptr);
        _renderPass = VK_NULL_HANDLE;
    }

    cleanupSwapchain();

    if (_device != VK_NULL_HANDLE) {
        vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
    }

    if (_debugMessenger != VK_NULL_HANDLE && _instance != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            _instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(_instance, _debugMessenger, nullptr);
        }
        _debugMessenger = VK_NULL_HANDLE;
    }

    if (_surface != VK_NULL_HANDLE && _instance != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        _surface = VK_NULL_HANDLE;
    }

    if (_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
}

void VulkanContext::cleanupSwapchain() {
    // Destroy framebuffers
    for (auto framebuffer : _swapchainFramebuffers) {
        vkDestroyFramebuffer(_device, framebuffer, nullptr);
    }
    _swapchainFramebuffers.clear();

    // Destroy depth resources
    if (_depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(_device, _depthImageView, nullptr);
        _depthImageView = VK_NULL_HANDLE;
    }
    if (_depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(_device, _depthImage, nullptr);
        _depthImage = VK_NULL_HANDLE;
    }
    if (_depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(_device, _depthImageMemory, nullptr);
        _depthImageMemory = VK_NULL_HANDLE;
    }

    // Destroy image views
    for (auto imageView : _swapchainImageViews) {
        vkDestroyImageView(_device, imageView, nullptr);
    }
    _swapchainImageViews.clear();

    // Destroy swapchain
    if (_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        _swapchain = VK_NULL_HANDLE;
    }
}

bool VulkanContext::createInstance() {
    if (_enableValidationLayers && !checkValidationLayerSupport()) {
        std::cout << "Validation layers not available - continuing without them" << std::endl;
        _enableValidationLayers = false;  // Disable them if not available
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "YARS";
    appInfo.applicationVersion = VK_MAKE_VERSION(2, 0, 0);
    appInfo.pEngineName = "YARS Vulkan Renderer";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;  // Use Vulkan 1.1 for better compatibility

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // For macOS, we need to enable portability enumeration
    VkInstanceCreateFlags flags = 0;
#ifdef __APPLE__
    flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    createInfo.flags = flags;

    if (_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    std::cout << "Creating Vulkan instance..." << std::endl;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create instance! Error code: " << result << std::endl;
        return false;
    }

    std::cout << "Vulkan instance created successfully!" << std::endl;
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "Vulkan validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

bool VulkanContext::setupDebugMessenger() {
    if (!_enableValidationLayers) return true;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        _instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        VkResult result = func(_instance, &createInfo, nullptr, &_debugMessenger);
        return result == VK_SUCCESS;
    } else {
        return false;
    }
}

bool VulkanContext::createSurface(SDL_Window* window) {
    if (!SDL_Vulkan_CreateSurface(window, _instance, &_surface)) {
        std::cerr << "Failed to create Vulkan surface: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool VulkanContext::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : _validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> VulkanContext::getRequiredExtensions() {
    uint32_t sdlExtensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(_window, &sdlExtensionCount, nullptr);

    std::vector<const char*> extensions(sdlExtensionCount);
    SDL_Vulkan_GetInstanceExtensions(_window, &sdlExtensionCount, extensions.data());

    if (_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Required for macOS portability
#ifdef __APPLE__
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

    return extensions;
}

bool VulkanContext::pickPhysicalDevice() {
    std::cout << "Enumerating physical devices..." << std::endl;
    uint32_t deviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to enumerate physical devices! Error: " << result << std::endl;
        return false;
    }

    std::cout << "Found " << deviceCount << " device(s)" << std::endl;
    if (deviceCount == 0) {
        std::cerr << "Failed to find GPUs with Vulkan support!" << std::endl;
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to get physical device list! Error: " << result << std::endl;
        return false;
    }

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            _physicalDevice = device;
            _queueFamilyIndices = findQueueFamilies(device);

            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            std::cout << "Selected GPU: " << properties.deviceName << std::endl;
            return true;
        }
    }

    std::cerr << "Failed to find a suitable GPU!" << std::endl;
    return false;
}

bool VulkanContext::isDeviceSuitable(VkPhysicalDevice device) {
    std::cout << "  Checking device suitability..." << std::endl;

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    std::cout << "  Device: " << properties.deviceName << std::endl;

    QueueFamilyIndices indices = findQueueFamilies(device);
    std::cout << "  Queue families checked" << std::endl;

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    std::cout << "  Extensions supported: " << extensionsSupported << std::endl;

    bool swapchainAdequate = false;
    if (extensionsSupported) {
        std::cout << "  Querying swapchain support..." << std::endl;
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device);
        swapchainAdequate = !swapchainSupport.formats.empty() &&
                            !swapchainSupport.presentModes.empty();
        std::cout << "  Swapchain adequate: " << swapchainAdequate << std::endl;
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    bool suitable = indices.isComplete() && extensionsSupported && swapchainAdequate &&
           supportedFeatures.samplerAnisotropy;
    std::cout << "  Device suitable: " << suitable << std::endl;
    return suitable;
}

VulkanContext::QueueFamilyIndices VulkanContext::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool VulkanContext::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool VulkanContext::createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        _queueFamilyIndices.graphicsFamily.value(),
        _queueFamilyIndices.presentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;  // For wireframe rendering if needed

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

    if (_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create logical device! Error: " << result << std::endl;
        return false;
    }

    vkGetDeviceQueue(_device, _queueFamilyIndices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, _queueFamilyIndices.presentFamily.value(), 0, &_presentQueue);

    return true;
}

VulkanContext::SwapchainSupportDetails VulkanContext::querySwapchainSupport(VkPhysicalDevice device) const {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanContext::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanContext::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width,
            capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

bool VulkanContext::createSwapchain() {
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {
        _queueFamilyIndices.graphicsFamily.value(),
        _queueFamilyIndices.presentFamily.value()
    };

    if (_queueFamilyIndices.graphicsFamily != _queueFamilyIndices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapchain);
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create swapchain! Error: " << result << std::endl;
        return false;
    }

    vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, nullptr);
    _swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, _swapchainImages.data());

    _swapchainImageFormat = surfaceFormat.format;
    _swapchainExtent = extent;

    std::cout << "Swapchain created: " << extent.width << "x" << extent.height << std::endl;
    return true;
}

bool VulkanContext::createImageViews() {
    _swapchainImageViews.resize(_swapchainImages.size());

    for (size_t i = 0; i < _swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = _swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(_device, &createInfo, nullptr, &_swapchainImageViews[i]);
        if (result != VK_SUCCESS) {
            std::cerr << "Failed to create image view " << i << "! Error: " << result << std::endl;
            return false;
        }
    }

    return true;
}

void VulkanContext::recreateSwapchain() {
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(_window, &width, &height);
    while (width == 0 || height == 0) {
        SDL_Vulkan_GetDrawableSize(_window, &width, &height);
        SDL_WaitEvent(nullptr);
    }

    vkDeviceWaitIdle(_device);

    cleanupSwapchain();

    createSwapchain();
    createImageViews();
    createDepthResources();
    createFramebuffers();
}

} // namespace yars

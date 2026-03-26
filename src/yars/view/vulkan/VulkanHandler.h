#ifndef __VULKAN_HANDLER_H__
#define __VULKAN_HANDLER_H__

#include <yars/view/vulkan/VulkanContext.h>
#include <yars/view/vulkan/VulkanSceneGraph.h>
#include <SDL2/SDL.h>

namespace yars {

/**
 * VulkanHandler - Singleton managing Vulkan rendering for YARS
 *
 * Parallels OgreHandler architecture:
 * - Manages VulkanContext lifecycle
 * - Coordinates scene graph updates with frame rendering
 * - Provides central access point for Vulkan rendering
 */
class VulkanHandler {
public:
    static VulkanHandler* instance();
    ~VulkanHandler();

    // Core rendering interface (matches OgreHandler)
    void step();   // Update scene graph and render one frame
    void reset();  // Reset scene graph state

    // Frame export
    bool exportFrame(const std::string& filename);

    // Initialization
    bool initialize(SDL_Window* window);
    void cleanup();

    // Getters
    VulkanContext* getContext() const { return _context; }
    VulkanSceneGraph* getSceneGraph() const { return _sceneGraph; }

    // Camera controls
    void handleMouseMotion(int dx, int dy, bool leftButton, bool rightButton);
    void handleKeyboard(const bool* keys, float deltaTime);
    void resetCamera();

private:
    VulkanHandler();
    VulkanHandler(const VulkanHandler&) = delete;
    VulkanHandler& operator=(const VulkanHandler&) = delete;

    static VulkanHandler* _me;

    VulkanContext* _context;
    VulkanSceneGraph* _sceneGraph;
    SDL_Window* _window;
};

} // namespace yars

#endif // __VULKAN_HANDLER_H__

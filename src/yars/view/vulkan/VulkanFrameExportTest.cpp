// Test application for frame export functionality
#include "VulkanHandler.h"
#include "VulkanBuffers.h"
#include "VulkanCamera.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace yars;

int main(int, char**) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "YARS Frame Export Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "\n=== VULKAN FRAME EXPORT TEST ===" << std::endl;

    // Initialize VulkanHandler
    VulkanHandler* handler = VulkanHandler::instance();
    if (!handler->initialize(window)) {
        std::cerr << "Failed to initialize VulkanHandler!" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "VulkanHandler initialized successfully!" << std::endl;

    // Get context for setting up geometry
    VulkanContext* ctx = handler->getContext();

    // Create test geometry (cube)
    VulkanBuffer buffer(ctx);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GeometryGenerator::generateCube(1.0f, vertices, indices);

    if (!buffer.createVertexBuffer(vertices)) {
        std::cerr << "Failed to create vertex buffer!" << std::endl;
        handler->cleanup();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!buffer.createIndexBuffer(indices)) {
        std::cerr << "Failed to create index buffer!" << std::endl;
        handler->cleanup();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set active geometry
    ctx->setActiveGeometry(buffer.getVertexBuffer(), buffer.getIndexBuffer(), buffer.getIndexCount());

    // Setup camera
    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.lookAt(glm::vec3(3, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    std::cout << "\n=== RENDERING AND EXPORTING FRAME ===" << std::endl;

    // Render a few frames to ensure everything is set up
    for (int i = 0; i < 10; i++) {
        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(22.5f), glm::vec3(1, 0, 0));

        // Update uniform buffer
        ctx->updateUniformBuffer(ctx->getCurrentFrame(), model, camera.getViewMatrix(), camera.getProjectionMatrix());

        // Render frame
        try {
            handler->step();
            std::cout << "Frame " << (i+1) << " rendered" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Rendering error: " << e.what() << std::endl;
            buffer.cleanup();
            handler->cleanup();
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }

        // Export frame 5
        if (i == 4) {
            std::cout << "\n=== EXPORTING FRAME ===" << std::endl;
            if (handler->exportFrame("vulkan_export.png")) {
                std::cout << "Frame exported successfully to vulkan_export.png" << std::endl;
            } else {
                std::cerr << "Failed to export frame!" << std::endl;
            }
        }

        // Small delay
        SDL_Delay(16);
    }

    std::cout << "\n=== TEST COMPLETE ===" << std::endl;

    // Cleanup
    buffer.cleanup();
    handler->cleanup();
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Frame export test completed successfully!" << std::endl;
    std::cout << "Check vulkan_export.png to verify rendering" << std::endl;
    return 0;
}

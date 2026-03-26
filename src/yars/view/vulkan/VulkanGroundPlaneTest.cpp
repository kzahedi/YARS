// Test program to show ground plane rendering
#include "VulkanContext.h"
#include "VulkanBuffers.h"
#include "VulkanCamera.h"
#include "VulkanTextureManager.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace yars;

int main(int, char**) {
    std::cout << "\n=== VULKAN GROUND PLANE TEST ===" << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Ground Plane Test",
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

    // Initialize Vulkan
    VulkanContext* ctx = VulkanContext::instance();
    if (!ctx->initialize(window)) {
        std::cerr << "Failed to initialize Vulkan!" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "Vulkan initialized!" << std::endl;

    // Initialize texture manager
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    texManager->initialize(ctx);
    texManager->addSearchPath("../../materials");
    std::cout << "Texture manager initialized" << std::endl;

    // Load ground texture
    VulkanTexture* groundTexture = texManager->getTexture("ground.jpg");
    if (!groundTexture || !groundTexture->isValid()) {
        std::cerr << "Warning: Could not load ground.jpg" << std::endl;
    } else {
        std::cout << "Loaded ground texture: " << groundTexture->getWidth() << "x" << groundTexture->getHeight() << std::endl;
    }

    // Create ground plane geometry (50x50 meters with subdivisions)
    VulkanBuffer groundBuffer(ctx);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GeometryGenerator::generatePlane(50.0f, 50.0f, 10, 10, vertices, indices);

    if (!groundBuffer.createVertexBuffer(vertices) || !groundBuffer.createIndexBuffer(indices)) {
        std::cerr << "Failed to create ground plane buffers!" << std::endl;
        VulkanContext::destroy();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "Created ground plane: " << vertices.size() << " vertices, "
              << indices.size() << " indices" << std::endl;

    // Create a box to show scale
    VulkanBuffer boxBuffer(ctx);
    std::vector<Vertex> boxVertices;
    std::vector<uint32_t> boxIndices;
    GeometryGenerator::generateCube(2.0f, boxVertices, boxIndices);

    if (!boxBuffer.createVertexBuffer(boxVertices) || !boxBuffer.createIndexBuffer(boxIndices)) {
        std::cerr << "Failed to create box buffers!" << std::endl;
    }

    // Load wall texture for the box
    VulkanTexture* wallTexture = texManager->getTexture("wall.jpg");

    // Setup camera to show ground plane from above at an angle (similar to braitenberg view)
    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 200.0f);
    camera.lookAt(glm::vec3(8, 6, 8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    std::cout << "\nRendering ground plane scene with reference box..." << std::endl;

    // Main render loop - render both ground and box
    int frameCount = 0;
    bool exported = false;

    for (int i = 0; i < 10 && !exported; i++) {
        // First render ground plane
        if (groundTexture && groundTexture->isValid()) {
            ctx->bindTexture(groundTexture->getImageView(), groundTexture->getSampler());
        }

        glm::mat4 groundModel = glm::mat4(1.0f);
        groundModel = glm::translate(groundModel, glm::vec3(0.0f, 0.0f, -0.01f));

        ctx->setActiveGeometry(groundBuffer.getVertexBuffer(), groundBuffer.getIndexBuffer(), groundBuffer.getIndexCount());
        ctx->updateUniformBuffer(ctx->getCurrentFrame(), groundModel, camera.getViewMatrix(), camera.getProjectionMatrix());

        try {
            ctx->drawFrame();
            frameCount++;

            // Wait a few frames for everything to stabilize, then export
            if (i == 3) {
                std::string filename = "ground_plane_with_scene.png";
                std::cout << "Exporting frame: " << filename << std::endl;

                if (ctx->exportFrame(filename)) {
                    std::cout << "✓ Exported: " << filename << std::endl;
                    exported = true;
                } else {
                    std::cout << "✗ Export failed" << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }

    std::cout << "\nRendered " << frameCount << " frames" << std::endl;
    std::cout << "Ground plane test complete!" << std::endl;

    // Cleanup
    groundBuffer.cleanup();
    boxBuffer.cleanup();
    VulkanContext::destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Export frames from simple scene with textures for visual inspection
// Similar to braitenberg but simplified for testing
#include "VulkanContext.h"
#include "VulkanBuffers.h"
#include "VulkanCamera.h"
#include "VulkanTextureManager.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace yars;

int main(int, char**) {
    std::cout << "\n=== VULKAN SCENE EXPORT TEST ===" << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Scene Export",
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

    // Load CBT texture (circuit board for robot body)
    VulkanTexture* texture = texManager->getTexture("CBT.jpg");
    if (texture && texture->isValid()) {
        std::cout << "Loaded CBT.jpg: " << texture->getWidth() << "x" << texture->getHeight() << std::endl;
        ctx->bindTexture(texture->getImageView(), texture->getSampler());
    } else {
        std::cerr << "Warning: Could not load CBT.jpg" << std::endl;
    }

    // Create test geometry (larger cube to simulate robot body)
    VulkanBuffer buffer(ctx);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GeometryGenerator::generateCube(2.5f, vertices, indices);

    if (!buffer.createVertexBuffer(vertices) || !buffer.createIndexBuffer(indices)) {
        std::cerr << "Failed to create geometry buffers!" << std::endl;
        VulkanContext::destroy();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ctx->setActiveGeometry(buffer.getVertexBuffer(), buffer.getIndexBuffer(), buffer.getIndexCount());

    // Setup camera (similar to braitenberg view)
    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.lookAt(glm::vec3(5, 4, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    std::cout << "\nRendering scene with textures..." << std::endl;
    std::cout << "Will export frames at intervals\n" << std::endl;

    // Main loop - export first 500 frames (every 10th frame = 50 exports)
    bool running = true;
    SDL_Event event;
    int frameCount = 0;
    float rotation = 0.0f;
    Uint32 lastTime = SDL_GetTicks();

    const int maxFrames = 500;
    const int exportInterval = 10;  // Export every 10th frame
    int exportCount = 0;

    std::cout << "Will render " << maxFrames << " frames, exporting every " << exportInterval << "th frame" << std::endl;

    while (running && frameCount < maxFrames) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        // Update rotation
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        rotation += deltaTime * 20.0f;

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation * 0.3f), glm::vec3(1, 0, 0));

        // Update and render
        ctx->updateUniformBuffer(ctx->getCurrentFrame(), model, camera.getViewMatrix(), camera.getProjectionMatrix());

        try {
            ctx->drawFrame();
            frameCount++;

            // Export every exportInterval frames (plus frame 1)
            if (frameCount == 1 || frameCount % exportInterval == 0) {
                std::string filename = "frame_" + std::to_string(frameCount) + ".png";

                if (ctx->exportFrame(filename)) {
                    exportCount++;
                    if (exportCount % 10 == 0) {
                        std::cout << "Progress: " << frameCount << "/" << maxFrames << " frames (" << exportCount << " exported)" << std::endl;
                    }
                } else {
                    std::cout << "✗ Export failed for frame " << frameCount << std::endl;
                }
            }

            if (frameCount % 100 == 0) {
                std::cout << "Rendered " << frameCount << " frames..." << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "\nTotal: " << frameCount << " frames rendered, " << exportCount << " exported" << std::endl;

    std::cout << "\nExported files: frame_1.png, frame_10.png, frame_20.png, ... frame_500.png" << std::endl;

    // Cleanup
    buffer.cleanup();
    VulkanTextureManager::destroy();
    VulkanContext::destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "\nView frames: open scene_*.png" << std::endl;

    return 0;
}

// Simple frame capture from Vulkan rendering
#include "VulkanContext.h"
#include "VulkanBuffers.h"
#include "VulkanCamera.h"
#include "VulkanTextureManager.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

using namespace yars;

int main(int, char**) {
    std::cout << "\n=== VULKAN FRAME CAPTURE TEST ===" << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Frame Capture",
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

    // Initialize texture manager and load CBT texture
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    texManager->initialize(ctx);
    texManager->addSearchPath("../../materials");

    VulkanTexture* texture = texManager->getTexture("CBT.jpg");
    if (texture && texture->isValid()) {
        std::cout << "Texture loaded: CBT.jpg (" << texture->getWidth() << "x" << texture->getHeight() << ")" << std::endl;
        ctx->bindTexture(texture->getImageView(), texture->getSampler());
    }

    // Create geometry (textured cube)
    VulkanBuffer buffer(ctx);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GeometryGenerator::generateCube(2.0f, vertices, indices);

    if (!buffer.createVertexBuffer(vertices)) {
        std::cerr << "Failed to create vertex buffer!" << std::endl;
        VulkanContext::destroy();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!buffer.createIndexBuffer(indices)) {
        std::cerr << "Failed to create index buffer!" << std::endl;
        VulkanContext::destroy();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ctx->setActiveGeometry(buffer.getVertexBuffer(), buffer.getIndexBuffer(), buffer.getIndexCount());

    // Setup camera
    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.lookAt(glm::vec3(4, 3, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    std::cout << "Scene setup complete!" << std::endl;
    std::cout << "Rendering and capturing frames...\n" << std::endl;

    // Main loop - render and capture frames
    bool running = true;
    SDL_Event event;
    int frameCount = 0;
    float rotation = 0.0f;
    Uint32 lastTime = SDL_GetTicks();

    int captureFrames[] = {30, 60, 90, 120, 150}; // Frame numbers to capture
    int nextCaptureIndex = 0;
    int numCaptures = 5;

    while (running && nextCaptureIndex < numCaptures) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Update rotation
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        rotation += deltaTime * 30.0f; // 30 degrees per second

        // Create model matrix (rotating cube)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation * 0.5f), glm::vec3(1, 0, 0));

        // Update uniform buffer
        ctx->updateUniformBuffer(ctx->getCurrentFrame(), model, camera.getViewMatrix(), camera.getProjectionMatrix());

        // Render frame
        try {
            ctx->drawFrame();
            frameCount++;

            // Capture frame at specific intervals
            if (nextCaptureIndex < numCaptures && frameCount == captureFrames[nextCaptureIndex]) {
                std::string filename = "capture_frame_" + std::to_string(nextCaptureIndex) + ".png";
                std::cout << "Frame " << frameCount << ": Capturing " << filename << "..." << std::endl;

                if (ctx->exportFrame(filename)) {
                    std::cout << "✓ Frame saved: " << filename << std::endl;
                    nextCaptureIndex++;
                } else {
                    std::cout << "✗ Failed to save frame" << std::endl;
                }
            }

            // Print progress
            if (frameCount % 30 == 0) {
                std::cout << "Rendered " << frameCount << " frames (rotation: " << (int)rotation % 360 << "°)" << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "Rendering error: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "\nTotal frames rendered: " << frameCount << std::endl;
    std::cout << "Total frames captured: " << nextCaptureIndex << std::endl;

    // Cleanup
    buffer.cleanup();
    VulkanTextureManager::destroy();
    VulkanContext::destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "\nCleanup complete!" << std::endl;
    std::cout << "\nYou can view the captured frames with:" << std::endl;
    std::cout << "  open capture_frame_*.png" << std::endl;

    return 0;
}

// Test application for VulkanHandler - validates integration between VulkanContext and VulkanSceneGraph
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
        "YARS VulkanHandler Test",
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

    std::cout << "\n=== VULKAN HANDLER TEST ===" << std::endl;

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

    std::cout << "\n=== STARTING RENDER LOOP ===" << std::endl;
    std::cout << "VulkanHandler::step() will be called each frame" << std::endl;
    std::cout << "Press ESC to quit\n" << std::endl;

    // Main loop
    bool running = true;
    SDL_Event event;
    float rotation = 0.0f;
    Uint32 lastTime = SDL_GetTicks();
    int frameCount = 0;

    while (running) {
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
        rotation += deltaTime * 45.0f;

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation * 0.5f), glm::vec3(1, 0, 0));

        // Update uniform buffer
        ctx->updateUniformBuffer(ctx->getCurrentFrame(), model, camera.getViewMatrix(), camera.getProjectionMatrix());

        // Use VulkanHandler::step() to render (matches YARS architecture)
        try {
            handler->step();
            frameCount++;

            if (frameCount % 60 == 0) {
                std::cout << "Frame: " << frameCount
                         << ", Rotation: " << (int)rotation % 360 << "°" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Rendering error: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "\n=== TEST COMPLETE ===" << std::endl;
    std::cout << "Total frames rendered: " << frameCount << std::endl;

    // Cleanup
    buffer.cleanup();
    handler->cleanup();
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "VulkanHandler test completed successfully!" << std::endl;
    return 0;
}

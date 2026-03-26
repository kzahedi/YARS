// Test application that renders a rotating 3D cube using geometry buffers
#include "VulkanContext.h"
#include "VulkanBuffers.h"
#include "VulkanCamera.h"
#include "VulkanTextureManager.h"
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
        "YARS Vulkan Geometry Test",
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

    std::cout << "\n=== VULKAN GEOMETRY TEST ===" << std::endl;

    // Initialize texture manager
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    texManager->initialize(ctx);
    texManager->addSearchPath("../../materials");
    texManager->addSearchPath("../materials");
    std::cout << "Texture manager initialized with " << texManager->getSearchPathCount()
              << " search paths" << std::endl;

    // Load CBT.jpg texture (circuit board texture from braitenberg robot)
    std::cout << "Loading CBT.jpg texture..." << std::endl;
    VulkanTexture* texture = texManager->getTexture("CBT.jpg");
    if (!texture || !texture->isValid()) {
        std::cerr << "WARNING: Failed to load CBT.jpg texture. "
                  << "Rendering will proceed without texture." << std::endl;
    } else {
        std::cout << "Texture loaded successfully: " << texture->getWidth()
                  << "x" << texture->getHeight() << std::endl;

        // Bind texture to descriptor sets
        ctx->bindTexture(texture->getImageView(), texture->getSampler());
        std::cout << "Texture bound to descriptor sets" << std::endl;
    }

    std::cout << "\nCreating 3D cube geometry..." << std::endl;

    // Create geometry buffers
    VulkanBuffer buffer(ctx);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Generate a cube
    GeometryGenerator::generateCube(1.0f, vertices, indices);

    std::cout << "Cube: " << vertices.size() << " vertices, "
              << indices.size() << " indices" << std::endl;

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

    std::cout << "Geometry buffers created successfully!" << std::endl;

    // Set active geometry for rendering
    ctx->setActiveGeometry(buffer.getVertexBuffer(), buffer.getIndexBuffer(), buffer.getIndexCount());

    // Setup camera
    Camera camera;
    camera.setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.lookAt(glm::vec3(3, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    std::cout << "Camera configured" << std::endl;
    std::cout << "\nRendering rotating cube..." << std::endl;
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
        rotation += deltaTime * 45.0f; // 45 degrees per second

        // Create model matrix (rotating cube)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation * 0.5f), glm::vec3(1, 0, 0));

        // Update uniform buffer with MVP matrices (use current frame index)
        ctx->updateUniformBuffer(ctx->getCurrentFrame(), model, camera.getViewMatrix(), camera.getProjectionMatrix());

        // Draw frame
        try {
            ctx->drawFrame();
            frameCount++;

            // Print FPS every second
            if (frameCount % 60 == 0) {
                std::cout << "Frame: " << frameCount
                         << ", Rotation: " << (int)rotation % 360 << "°" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Rendering error: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "\nTotal frames rendered: " << frameCount << std::endl;

    // Cleanup
    buffer.cleanup();
    VulkanTextureManager::destroy();
    VulkanContext::destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Cleanup successful!" << std::endl;
    return 0;
}

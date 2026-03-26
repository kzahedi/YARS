// Minimal Vulkan test - clears screen to blue
#include "VulkanContext.h"
#include <SDL2/SDL.h>
#include <iostream>

using namespace yars;

int main(int argc, char** argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "YARS Vulkan Test",
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

    std::cout << "\n=== VULKAN INITIALIZATION SUCCESSFUL ===" << std::endl;
    std::cout << "Swapchain format: " << ctx->getSwapchainImageFormat() << std::endl;
    std::cout << "Swapchain extent: "
              << ctx->getSwapchainExtent().width << "x"
              << ctx->getSwapchainExtent().height << std::endl;
    std::cout << "Swapchain images: " << ctx->getSwapchainImageViews().size() << std::endl;
    std::cout << "\nPress ESC to quit" << std::endl;

    // Main loop
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Draw frame
        try {
            ctx->drawFrame();
        } catch (const std::exception& e) {
            std::cerr << "Rendering error: " << e.what() << std::endl;
            running = false;
        }
    }

    // Cleanup
    VulkanContext::destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "\nCleanup successful!" << std::endl;
    return 0;
}

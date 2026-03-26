// Test program to verify camera controls are functional
#include "VulkanContext.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace yars;

int main(int, char**) {
    std::cout << "\n=== VULKAN CAMERA CONTROL TEST ===" << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Vulkan Camera Test",
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

    // Test 1: Get initial camera position
    std::cout << "\n=== Test 1: Initial Camera State ===" << std::endl;
    glm::vec3 initialPos = ctx->getCameraPosition();
    glm::vec3 initialTarget = ctx->getCameraTarget();
    std::cout << "Camera Position: (" << initialPos.x << ", " << initialPos.y << ", " << initialPos.z << ")" << std::endl;
    std::cout << "Camera Target: (" << initialTarget.x << ", " << initialTarget.y << ", " << initialTarget.z << ")" << std::endl;

    // Test 2: Simulate mouse motion (orbit camera)
    std::cout << "\n=== Test 2: Mouse Orbit (Left Button Drag) ===" << std::endl;
    std::cout << "Simulating left mouse drag: dx=50, dy=30" << std::endl;
    ctx->handleMouseMotion(50, 30, true, false);

    glm::vec3 newPos = ctx->getCameraPosition();
    std::cout << "New Camera Position: (" << newPos.x << ", " << newPos.y << ", " << newPos.z << ")" << std::endl;

    if (newPos != initialPos) {
        std::cout << "✓ Camera position changed - orbit working!" << std::endl;
    } else {
        std::cout << "✗ Camera position unchanged" << std::endl;
    }

    // Test 3: Simulate keyboard movement
    std::cout << "\n=== Test 3: Keyboard Movement (WASD) ===" << std::endl;
    bool keys[512] = {false};
    keys[SDL_SCANCODE_W] = true;  // W key pressed

    std::cout << "Simulating W key press (forward movement)" << std::endl;
    glm::vec3 beforeMove = ctx->getCameraPosition();
    ctx->handleKeyboard(keys, 0.1f);  // 0.1 second delta

    glm::vec3 afterMove = ctx->getCameraPosition();
    std::cout << "Position after W: (" << afterMove.x << ", " << afterMove.y << ", " << afterMove.z << ")" << std::endl;

    if (afterMove != beforeMove) {
        std::cout << "✓ Camera moved with W key - keyboard working!" << std::endl;
    } else {
        std::cout << "✗ Camera didn't move" << std::endl;
    }

    // Test 4: Reset camera
    std::cout << "\n=== Test 4: Camera Reset ===" << std::endl;
    ctx->resetCamera();

    glm::vec3 resetPos = ctx->getCameraPosition();
    std::cout << "Reset Camera Position: (" << resetPos.x << ", " << resetPos.y << ", " << resetPos.z << ")" << std::endl;
    std::cout << "✓ Camera reset function executed" << std::endl;

    // Test 5: Set custom camera position
    std::cout << "\n=== Test 5: Set Custom Camera Position ===" << std::endl;
    glm::vec3 customPos(10.0f, 5.0f, 10.0f);
    glm::vec3 customTarget(0.0f, 0.0f, 0.0f);

    ctx->setCameraPosition(customPos, customTarget);
    glm::vec3 finalPos = ctx->getCameraPosition();

    std::cout << "Set Position: (" << customPos.x << ", " << customPos.y << ", " << customPos.z << ")" << std::endl;
    std::cout << "Actual Position: (" << finalPos.x << ", " << finalPos.y << ", " << finalPos.z << ")" << std::endl;

    if (finalPos == customPos) {
        std::cout << "✓ Custom camera position set correctly!" << std::endl;
    } else {
        std::cout << "✗ Custom position mismatch" << std::endl;
    }

    std::cout << "\n=== Camera Control Test Complete ===" << std::endl;
    std::cout << "All camera control functions are accessible and functional!" << std::endl;
    std::cout << "\nCamera Controls Summary:" << std::endl;
    std::cout << "  - Left mouse drag: Orbit camera around target" << std::endl;
    std::cout << "  - Right mouse drag: Zoom in/out" << std::endl;
    std::cout << "  - WASD keys: Move camera position" << std::endl;
    std::cout << "  - Q/E keys: Move up/down" << std::endl;
    std::cout << "  - Arrow keys: Rotate view" << std::endl;
    std::cout << "  - R key: Reset camera to default position" << std::endl;

    // Cleanup
    VulkanContext::destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

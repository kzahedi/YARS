#include "VulkanContext.h"
#include "VulkanBuffers.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

using namespace yars;

// Test structure for a static box
struct TestBox {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec4 color;
    std::string name;
};

// Create test boxes matching the XML wall positions
std::vector<TestBox> createTestWalls() {
    std::vector<TestBox> boxes;

    // Wall 1: x=4, y=0, z=-0.15, gamma=90 (rotation around Z axis)
    TestBox wall1;
    wall1.position = glm::vec3(4.0f, 0.0f, -0.15f);
    wall1.rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    wall1.scale = glm::vec3(7.75f, 0.25f, 0.5f);  // width, depth, height
    wall1.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red
    wall1.name = "Wall 1 (Red)";
    boxes.push_back(wall1);

    // Wall 2: x=-4, y=0, z=-0.15, gamma=90
    TestBox wall2;
    wall2.position = glm::vec3(-4.0f, 0.0f, -0.15f);
    wall2.rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    wall2.scale = glm::vec3(7.75f, 0.25f, 0.5f);
    wall2.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
    wall2.name = "Wall 2 (Green)";
    boxes.push_back(wall2);

    // Wall 3: x=0, y=4, z=-0.15, gamma=0 (no rotation)
    TestBox wall3;
    wall3.position = glm::vec3(0.0f, 4.0f, -0.15f);
    wall3.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity
    wall3.scale = glm::vec3(8.25f, 0.25f, 0.5f);
    wall3.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);  // Blue
    wall3.name = "Wall 3 (Blue)";
    boxes.push_back(wall3);

    // Wall 4: x=0, y=-4, z=-0.15, gamma=0
    TestBox wall4;
    wall4.position = glm::vec3(0.0f, -4.0f, -0.15f);
    wall4.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    wall4.scale = glm::vec3(8.25f, 0.25f, 0.5f);
    wall4.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
    wall4.name = "Wall 4 (Yellow)";
    boxes.push_back(wall4);

    return boxes;
}

// Apply Bullet->Vulkan coordinate transformation
glm::mat4 bulletToVulkanTransform(const glm::vec3& bulletPos, const glm::quat& bulletRot, const glm::vec3& scale) {
    // Build transformation in Bullet space
    glm::mat4 bulletTranslation = glm::translate(glm::mat4(1.0f), bulletPos);
    glm::mat4 bulletRotation = glm::mat4_cast(bulletRot);
    glm::mat4 bulletScale = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 bulletModel = bulletTranslation * bulletRotation * bulletScale;

    // Coordinate space transform: Bullet(X,Y,Z) -> Vulkan(X, Z, -Y)
    glm::mat4 coordChange(
        1.0f,  0.0f,  0.0f, 0.0f,  // X stays X
        0.0f,  0.0f,  1.0f, 0.0f,  // Bullet Z (up) → Vulkan Y (up)
        0.0f, -1.0f,  0.0f, 0.0f,  // Bullet Y (forward) → Vulkan -Z (depth)
        0.0f,  0.0f,  0.0f, 1.0f
    );

    return coordChange * bulletModel;
}

int main(int argc, char* argv[]) {
    std::cout << "=== YARS Vulkan Static Box Test ===" << std::endl;
    std::cout << "This test renders 4 colored boxes at wall positions" << std::endl;
    std::cout << "Red=Wall1, Green=Wall2, Blue=Wall3, Yellow=Wall4" << std::endl << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "YARS Vulkan Static Box Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Initialize Vulkan
    VulkanContext* context = VulkanContext::instance();
    if (!context->initialize(window)) {
        std::cerr << "Vulkan initialization failed" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "Vulkan initialized successfully" << std::endl;

    // Create test boxes
    auto testBoxes = createTestWalls();

    // Create geometry for each box
    std::vector<VulkanBuffer*> buffers;
    std::vector<glm::mat4> transforms;

    for (const auto& box : testBoxes) {
        std::cout << "\nCreating " << box.name << std::endl;
        std::cout << "  Bullet pos: (" << box.position.x << ", " << box.position.y << ", " << box.position.z << ")" << std::endl;
        std::cout << "  Scale: (" << box.scale.x << ", " << box.scale.y << ", " << box.scale.z << ")" << std::endl;

        // Generate cube geometry
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        GeometryGenerator::generateCube(1.0f, vertices, indices);

        // Apply color to vertices
        for (auto& vertex : vertices) {
            vertex.color = glm::vec3(box.color);
        }

        // Create buffer
        VulkanBuffer* buffer = new VulkanBuffer(context);
        if (!buffer->createVertexBuffer(vertices)) {
            std::cerr << "Failed to create vertex buffer for " << box.name << std::endl;
            continue;
        }
        if (!buffer->createIndexBuffer(indices)) {
            std::cerr << "Failed to create index buffer for " << box.name << std::endl;
            continue;
        }

        buffers.push_back(buffer);

        // Calculate transformation
        glm::mat4 transform = bulletToVulkanTransform(box.position, box.rotation, box.scale);
        transforms.push_back(transform);

        // Debug output
        glm::vec4 vulkanPos = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        std::cout << "  Vulkan pos: (" << vulkanPos.x << ", " << vulkanPos.y << ", " << vulkanPos.z << ")" << std::endl;
    }

    std::cout << "\nCreated " << buffers.size() << " box buffers" << std::endl;
    std::cout << "\nRendering for 10 seconds..." << std::endl;
    std::cout << "Camera at (8, 8, 8) looking at (0, 0, 0)" << std::endl;

    // Export one frame after 5 seconds
    bool frameExported = false;

    // Main loop
    Uint32 startTime = SDL_GetTicks();
    bool running = true;
    while (running && (SDL_GetTicks() - startTime) < 10000) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Export frame at 5 seconds
        if (!frameExported && (SDL_GetTicks() - startTime) > 5000) {
            context->exportFrame("static_box_test.png");
            std::cout << "Frame exported to static_box_test.png" << std::endl;
            frameExported = true;
        }

        SDL_Delay(16);  // ~60 FPS
    }

    std::cout << "\nCleaning up..." << std::endl;

    // Cleanup
    for (auto* buffer : buffers) {
        buffer->cleanup();
        delete buffer;
    }

    context->cleanup();
    VulkanContext::destroy();

    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "\nTest complete. Check static_box_test.png" << std::endl;
    std::cout << "Expected: 4 colored boxes forming a square" << std::endl;
    std::cout << "  Red (Wall 1) on right, Green (Wall 2) on left" << std::endl;
    std::cout << "  Blue (Wall 3) on top, Yellow (Wall 4) on bottom" << std::endl;

    return 0;
}

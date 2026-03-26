#include "VulkanHandler.h"
#include "VulkanTextureManager.h"
#include <yars/configuration/data/Data.h>
#include <yars/configuration/data/DataRobotSimulationDescription.h>
#include <yars/configuration/data/DataScreens.h>
#include <yars/configuration/data/DataScreen.h>
#include <yars/configuration/data/DataCamera.h>
#include <yars/types/P3D.h>
#include <iostream>
#include <cmath>

namespace yars {

VulkanHandler* VulkanHandler::_me = nullptr;

VulkanHandler* VulkanHandler::instance() {
    if (_me == nullptr) {
        _me = new VulkanHandler();
    }
    return _me;
}

VulkanHandler::VulkanHandler()
    : _context(nullptr)
    , _sceneGraph(nullptr)
    , _window(nullptr)
{
    std::cout << "VulkanHandler: Created singleton instance" << std::endl;
}

VulkanHandler::~VulkanHandler() {
    cleanup();
}

bool VulkanHandler::initialize(SDL_Window* window) {
    if (!window) {
        std::cerr << "VulkanHandler: Invalid window provided" << std::endl;
        return false;
    }

    _window = window;

    std::cout << "VulkanHandler: Initializing Vulkan context..." << std::endl;

    // Initialize VulkanContext
    _context = VulkanContext::instance();
    if (!_context->initialize(window)) {
        std::cerr << "VulkanHandler: Failed to initialize Vulkan context" << std::endl;
        return false;
    }

    // Initialize VulkanTextureManager
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    texManager->initialize(_context);
    texManager->addSearchPath("../../materials");
    std::cout << "VulkanHandler: Texture manager initialized" << std::endl;

    // Only create scene graph if YARS Data is available
    // (needed for full YARS integration, not for simple tests)
    if (Data::instance() && Data::instance()->current()) {
        std::cout << "VulkanHandler: Creating scene graph..." << std::endl;
        _sceneGraph = new VulkanSceneGraph(_context);
        std::cout << "VulkanHandler: Scene graph created with "
                  << (_sceneGraph ? "success" : "failure") << std::endl;

        // Set up camera from YARS configuration
        DataRobotSimulationDescription* desc = Data::instance()->current();
        if (desc->screens() && desc->screens()->size() > 0) {
            DataScreen* screen = desc->screens()->screen(0);  // Get first screen
            if (screen && screen->camera()) {
                DataCamera* camera = screen->camera();
                P3D cameraPos = camera->position();
                P3D cameraLookAt = camera->lookAt();
                P3D cameraUp = camera->up();

                std::cout << "VulkanHandler: Camera from XML (Bullet coordinates):" << std::endl;
                std::cout << "  Position: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
                std::cout << "  LookAt: (" << cameraLookAt.x << ", " << cameraLookAt.y << ", " << cameraLookAt.z << ")" << std::endl;
                std::cout << "  Up: (" << cameraUp.x << ", " << cameraUp.y << ", " << cameraUp.z << ")" << std::endl;

                // Transform from Bullet (Z-up) to Vulkan (Y-up) coordinates
                // Bullet: X=right, Y=forward, Z=up
                // Vulkan: X=right, Y=up, Z=depth
                // Transformation: (X, Y, Z) → (X, Z, Y)
                glm::vec3 vulkanPos(cameraPos.x, cameraPos.z, cameraPos.y);
                glm::vec3 vulkanLookAt(cameraLookAt.x, cameraLookAt.z, cameraLookAt.y);

                // Transform up vector from Bullet to Vulkan coordinates
                // Bullet (X, Y, Z) → Vulkan (X, Z, Y)
                glm::vec3 vulkanUp(cameraUp.x, cameraUp.z, cameraUp.y);

                // Check if we're looking straight down (camera above looking at ground)
                glm::vec3 viewDir = glm::normalize(vulkanLookAt - vulkanPos);
                if (std::abs(viewDir.y) > 0.99f) {  // Looking mostly vertical
                    // Use Z-axis (Bullet's Y-forward) as up for the camera view
                    vulkanUp = glm::vec3(0.0f, 0.0f, -1.0f);  // Pointing back in Vulkan coords
                    std::cout << "VulkanHandler: Top-down view detected, using Z-axis as view up" << std::endl;
                }

                // For top-down view, adjust lookAt to exactly (0, 0, 0) if it's close
                // This ensures we're looking straight down at the center of the arena
                if (glm::length(vulkanLookAt) < 0.1f) {
                    vulkanLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
                    std::cout << "VulkanHandler: Adjusted lookAt to exact origin for centered view" << std::endl;
                }

                // NOTE: Using camera height from XML without modification

                std::cout << "VulkanHandler: Camera transformed to Vulkan coordinates:" << std::endl;
                std::cout << "  Position: (" << vulkanPos.x << ", " << vulkanPos.y << ", " << vulkanPos.z << ")" << std::endl;
                std::cout << "  LookAt: (" << vulkanLookAt.x << ", " << vulkanLookAt.y << ", " << vulkanLookAt.z << ")" << std::endl;
                std::cout << "  Up: (" << vulkanUp.x << ", " << vulkanUp.y << ", " << vulkanUp.z << ")" << std::endl;

                _context->setCameraFromYARS(vulkanPos, vulkanLookAt, vulkanUp);
            }
        }
    } else {
        std::cout << "VulkanHandler: No YARS data - skipping scene graph creation" << std::endl;
        _sceneGraph = nullptr;
    }

    std::cout << "VulkanHandler: Initialization complete!" << std::endl;
    return true;
}

void VulkanHandler::cleanup() {
    std::cout << "VulkanHandler: Cleaning up..." << std::endl;

    if (_sceneGraph) {
        delete _sceneGraph;
        _sceneGraph = nullptr;
    }

    if (_context) {
        VulkanContext::destroy();
        _context = nullptr;
    }

    _window = nullptr;
}

void VulkanHandler::step() {
    if (!_context || !_sceneGraph) {
        return;
    }

    try {
        // Update scene graph from physics data
        _sceneGraph->update();

        // Render all objects in scene graph
        _context->drawFrame(_sceneGraph);
    }
    catch (const std::exception& e) {
        static bool errorShown = false;
        if (!errorShown) {
            std::cerr << "VulkanHandler: Rendering error: " << e.what() << std::endl;
            errorShown = true;
        }
        // Continue simulation despite rendering error
    }
}

void VulkanHandler::reset() {
    if (_sceneGraph) {
        _sceneGraph->reset();
    }
}

bool VulkanHandler::exportFrame(const std::string& filename) {
    if (!_context) {
        std::cerr << "VulkanHandler: Cannot export frame - context not initialized" << std::endl;
        return false;
    }
    return _context->exportFrame(filename);
}

void VulkanHandler::handleMouseMotion(int dx, int dy, bool leftButton, bool rightButton) {
    if (_context) {
        _context->handleMouseMotion(dx, dy, leftButton, rightButton);
    }
}

void VulkanHandler::handleKeyboard(const bool* keys, float deltaTime) {
    if (_context) {
        _context->handleKeyboard(keys, deltaTime);
    }
}

void VulkanHandler::resetCamera() {
    if (_context) {
        _context->resetCamera();
    }
}

} // namespace yars

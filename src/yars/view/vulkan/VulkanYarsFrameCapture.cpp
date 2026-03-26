// Capture frames from actual YARS simulation
#include "VulkanHandler.h"
#include "../../configuration/YarsConfiguration.h"
#include "../../main/YarsMainControl.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace yars;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <xml_file> [num_frames]" << std::endl;
        return 1;
    }

    std::string xmlFile = argv[1];
    int numFrames = (argc > 2) ? std::atoi(argv[2]) : 500;

    std::cout << "=== YARS Frame Capture ===" << std::endl;
    std::cout << "XML: " << xmlFile << std::endl;
    std::cout << "Frames to capture: " << numFrames << std::endl;

    // Initialize YARS
    YarsConfiguration::init(argc, argv);

    if (!YarsConfiguration::instance()->simulatorGiven()) {
        std::cerr << "No simulator configuration found in XML" << std::endl;
        return 1;
    }

    // Create main control
    YarsMainControl* control = new YarsMainControl();

    // Get Vulkan handler
    VulkanHandler* vulkan = VulkanHandler::instance();

    std::cout << "Starting simulation..." << std::endl;

    // Run simulation and capture frames
    for (int frame = 0; frame < numFrames; frame++) {
        // Step simulation
        control->step();

        // Export every 10th frame
        if (frame % 10 == 0) {
            std::ostringstream filename;
            filename << "frame_" << std::setw(5) << std::setfill('0') << frame << ".png";

            if (vulkan->exportFrame(filename.str())) {
                std::cout << "Exported frame " << frame << " -> " << filename.str() << std::endl;
            } else {
                std::cerr << "Failed to export frame " << frame << std::endl;
            }
        }

        // Progress indicator
        if (frame % 50 == 0 && frame > 0) {
            std::cout << "Progress: " << frame << "/" << numFrames << " frames" << std::endl;
        }
    }

    std::cout << "Capture complete!" << std::endl;

    delete control;
    return 0;
}

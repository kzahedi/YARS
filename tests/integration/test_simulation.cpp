#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// Helper to get project root directory
static fs::path getProjectRoot() {
    // Start from current working directory and find YARS root
    fs::path current = fs::current_path();
    while (!current.empty() && current != current.root_path()) {
        if (fs::exists(current / "xml" / "braitenberg.xml")) {
            return current;
        }
        current = current.parent_path();
    }
    // Fallback: assume we're in build directory
    return fs::current_path().parent_path();
}

TEST_CASE("XML configuration files exist", "[Integration][XML]") {
    fs::path root = getProjectRoot();

    SECTION("braitenberg.xml exists") {
        REQUIRE(fs::exists(root / "xml" / "braitenberg.xml"));
    }

    SECTION("braitenberg_zoo.xml exists") {
        REQUIRE(fs::exists(root / "xml" / "braitenberg_zoo.xml"));
    }

    SECTION("braitenberg_logging.xml exists") {
        REQUIRE(fs::exists(root / "xml" / "braitenberg_logging.xml"));
    }
}

TEST_CASE("Multi-robot XML contains expected robots", "[Integration][MultiRobot]") {
    fs::path root = getProjectRoot();
    fs::path xml_path = root / "xml" / "braitenberg_zoo.xml";

    REQUIRE(fs::exists(xml_path));

    std::ifstream file(xml_path);
    REQUIRE(file.is_open());

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    SECTION("Contains Braitenberg 2a robot") {
        REQUIRE(content.find("Braitenberg2a") != std::string::npos);
    }

    SECTION("Contains Braitenberg 2b robot") {
        REQUIRE(content.find("Braitenberg2b") != std::string::npos);
    }

    SECTION("Contains Braitenberg 3a robot") {
        REQUIRE(content.find("Braitenberg3a") != std::string::npos);
    }

    SECTION("Contains Braitenberg 3b robot") {
        REQUIRE(content.find("Braitenberg3b") != std::string::npos);
    }

    SECTION("Contains light sources") {
        REQUIRE(content.find("pointLightSource") != std::string::npos);
    }

    SECTION("Contains multiple wheel hinges") {
        // Each robot should have 2 wheel hinges
        size_t hinge_count = 0;
        size_t pos = 0;
        while ((pos = content.find("wheel", pos)) != std::string::npos) {
            hinge_count++;
            pos++;
        }
        REQUIRE(hinge_count >= 8);  // At least 2 wheels per 4 robots
    }
}

TEST_CASE("Controller libraries exist", "[Integration][Controllers]") {
    fs::path root = getProjectRoot();
    fs::path lib_dir = root / "build" / "lib";

    // Skip if build dir doesn't exist (e.g., running from different location)
    if (!fs::exists(lib_dir)) {
        SKIP("Build library directory not found");
    }

    SECTION("Braitenberg2a controller exists") {
        bool found = fs::exists(lib_dir / "libYarsControllerBraitenberg2a.dylib") ||
                     fs::exists(lib_dir / "libYarsControllerBraitenberg2a.so");
        REQUIRE(found);
    }

    SECTION("Braitenberg2b controller exists") {
        bool found = fs::exists(lib_dir / "libYarsControllerBraitenberg2b.dylib") ||
                     fs::exists(lib_dir / "libYarsControllerBraitenberg2b.so");
        REQUIRE(found);
    }

    SECTION("Braitenberg3a controller exists") {
        bool found = fs::exists(lib_dir / "libYarsControllerBraitenberg3a.dylib") ||
                     fs::exists(lib_dir / "libYarsControllerBraitenberg3a.so");
        REQUIRE(found);
    }

    SECTION("Braitenberg3b controller exists") {
        bool found = fs::exists(lib_dir / "libYarsControllerBraitenberg3b.dylib") ||
                     fs::exists(lib_dir / "libYarsControllerBraitenberg3b.so");
        REQUIRE(found);
    }
}

TEST_CASE("Material files exist", "[Integration][Materials]") {
    fs::path root = getProjectRoot();
    fs::path materials_dir = root / "materials";

    SECTION("Chain.material exists") {
        REQUIRE(fs::exists(materials_dir / "Chain.material"));
    }

    SECTION("YARS.material exists") {
        REQUIRE(fs::exists(materials_dir / "YARS.material"));
    }
}

TEST_CASE("Texture files exist", "[Integration][Textures]") {
    fs::path root = getProjectRoot();
    fs::path materials_dir = root / "materials";

    SECTION("wheel.jpg exists") {
        REQUIRE(fs::exists(materials_dir / "wheel.jpg"));
    }

    SECTION("ground.jpg exists") {
        REQUIRE(fs::exists(materials_dir / "ground.jpg"));
    }
}

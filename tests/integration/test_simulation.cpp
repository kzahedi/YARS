#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

// YARS_PROJECT_ROOT is baked in by CMake (see tests/CMakeLists.txt) so the
// tests resolve project paths reliably regardless of where the binary is
// invoked from. Walking up from cwd doesn't work because build/xml is a
// symlink to xml/ and triggers a false positive at the build dir.
static fs::path getProjectRoot()
{
  return fs::path(YARS_PROJECT_ROOT);
}

TEST(IntegrationJson, BraitenbergJsonExists)
{
  EXPECT_TRUE(fs::exists(getProjectRoot() / "xml" / "braitenberg.json"));
}

TEST(IntegrationJson, BraitenbergZooJsonExists)
{
  EXPECT_TRUE(fs::exists(getProjectRoot() / "xml" / "braitenberg_zoo.json"));
}

TEST(IntegrationJson, BraitenbergLoggingJsonExists)
{
  EXPECT_TRUE(fs::exists(getProjectRoot() / "xml" / "braitenberg_logging.json"));
}

TEST(IntegrationMultiRobot, ZooJsonContainsExpectedRobotsAndLights)
{
  fs::path json_path = getProjectRoot() / "xml" / "braitenberg_zoo.json";
  ASSERT_TRUE(fs::exists(json_path));

  std::ifstream file(json_path);
  ASSERT_TRUE(file.is_open());

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  EXPECT_NE(content.find("Braitenberg2a"), std::string::npos);
  EXPECT_NE(content.find("Braitenberg2b"), std::string::npos);
  EXPECT_NE(content.find("Braitenberg3a"), std::string::npos);
  EXPECT_NE(content.find("Braitenberg3b"), std::string::npos);
  EXPECT_NE(content.find("pointLightSource"), std::string::npos);

  size_t hinge_count = 0;
  size_t pos = 0;
  while ((pos = content.find("wheel", pos)) != std::string::npos)
  {
    hinge_count++;
    pos++;
  }
  EXPECT_GE(hinge_count, 8u);
}

TEST(IntegrationControllers, BraitenbergControllerLibsExist)
{
  fs::path lib_dir = getProjectRoot() / "build" / "lib";
  if (!fs::exists(lib_dir))
  {
    GTEST_SKIP() << "Build library directory not found";
  }

  for (const std::string name : {"2a", "2b", "3a", "3b"})
  {
    bool found =
        fs::exists(lib_dir / ("libYarsControllerBraitenberg" + name + ".dylib")) ||
        fs::exists(lib_dir / ("libYarsControllerBraitenberg" + name + ".so"));
    EXPECT_TRUE(found) << "Missing Braitenberg" << name << " controller library";
  }
}

TEST(IntegrationMaterials, MaterialFilesExist)
{
  fs::path materials_dir = getProjectRoot() / "materials";
  EXPECT_TRUE(fs::exists(materials_dir / "Chain.material"));
  EXPECT_TRUE(fs::exists(materials_dir / "YARS.material"));
}

TEST(IntegrationTextures, TextureFilesExist)
{
  fs::path materials_dir = getProjectRoot() / "materials";
  EXPECT_TRUE(fs::exists(materials_dir / "wheel.jpg"));
  EXPECT_TRUE(fs::exists(materials_dir / "ground.jpg"));
}

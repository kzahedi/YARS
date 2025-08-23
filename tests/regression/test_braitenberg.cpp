#include <gtest/gtest.h>
#include <yars/types/P3D.h>
#include <yars/types/Pose.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdlib>

// Simplified regression test avoiding complex dependencies for now

class BraitenbergRegressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary working directory for test output
        test_output_dir = std::filesystem::temp_directory_path() / "yars_test";
        std::filesystem::create_directories(test_output_dir);
        
        // Change to the test directory
        original_dir = std::filesystem::current_path();
        std::filesystem::current_path(test_output_dir);
    }

    void TearDown() override {
        // Restore original directory
        std::filesystem::current_path(original_dir);
        
        // Clean up test directory (optional - you might want to keep for debugging)
        // std::filesystem::remove_all(test_output_dir);
    }

    std::filesystem::path test_output_dir;
    std::filesystem::path original_dir;
};

TEST_F(BraitenbergRegressionTest, BasicDataTypesWork) {
    // Simplified test that verifies basic data types work
    // This ensures the basic infrastructure is functional
    
    P3D point(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(point.x, 1.0);
    EXPECT_DOUBLE_EQ(point.y, 2.0);
    EXPECT_DOUBLE_EQ(point.z, 3.0);
    
    // Test that we can do basic operations
    P3D point2(4.0, 5.0, 6.0);
    P3D result = point + point2;
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 7.0);
    EXPECT_DOUBLE_EQ(result.z, 9.0);
}

TEST_F(BraitenbergRegressionTest, ConfigurationFileExists) {
    // Test that the braitenberg configuration file exists
    std::string xml_path = (original_dir / ".." / "xml" / "braitenberg_logging.xml").string();
    
    EXPECT_TRUE(std::filesystem::exists(xml_path)) << "Braitenberg XML file not found: " << xml_path;
    
    // Test that we can read the file
    std::ifstream file(xml_path);
    EXPECT_TRUE(file.is_open()) << "Could not open braitenberg XML file";
    
    // Check that it contains expected XML content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("<yars>") != std::string::npos) << "XML file does not contain expected YARS root element";
    EXPECT_TRUE(content.find("braitenberg") != std::string::npos) << "XML file does not contain braitenberg elements";
}
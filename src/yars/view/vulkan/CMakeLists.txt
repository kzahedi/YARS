find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

# SDL2 configuration (use direct path like main YARS build)
set(SDL2_INCLUDE_DIR /opt/homebrew/include/SDL2)
set(SDL2_LIBRARY /opt/homebrew/lib/libSDL2.dylib)

# Vulkan library
add_library(YarsVulkan STATIC
    VulkanContext.cpp
    VulkanRendering.cpp
    VulkanDepth.cpp
    VulkanBuffers.cpp
    VulkanCamera.cpp
    VulkanUniforms.cpp
    VulkanFrameExport.cpp
    VulkanHandler.cpp
    VulkanSceneGraph.cpp
    VulkanSceneNode.cpp
    VulkanBoxNode.cpp
    VulkanSphereNode.cpp
    VulkanCylinderNode.cpp
    VulkanCapsuleNode.cpp
    VulkanPlyNode.cpp
    VulkanCompositeNode.cpp
    VulkanTexture.cpp
    VulkanTextureManager.cpp
    VulkanTraceLineObject.cpp
    VulkanEnvironmentNode.cpp
    VulkanGroundPlaneNode.cpp
    VulkanRobotNode.cpp
)

target_include_directories(YarsVulkan PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${Vulkan_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/ext/stb  # stb_image header-only library
)

target_link_libraries(YarsVulkan PUBLIC
    ${Vulkan_LIBRARIES}
    ${SDL2_LIBRARY}
    glm::glm
)

target_compile_features(YarsVulkan PUBLIC cxx_std_17)

# Compile shaders
add_subdirectory(shaders)

# Vulkan test executable
add_executable(vulkan_test
    VulkanTest.cpp
)

target_link_libraries(vulkan_test PRIVATE
    YarsVulkan
)

add_dependencies(vulkan_test compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_test>/shaders
)

# Vulkan geometry test executable
add_executable(vulkan_geometry_test
    VulkanGeometryTest.cpp
)

target_link_libraries(vulkan_geometry_test PRIVATE
    YarsVulkan
    YarsConfiguration
    YarsDataTypes
    YarsUtil
    YarsViewConsole
    YarsGUI
)

add_dependencies(vulkan_geometry_test compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_geometry_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_geometry_test>/shaders
)

# Vulkan static box test executable
add_executable(vulkan_static_box_test
    VulkanStaticBoxTest.cpp
)

target_link_libraries(vulkan_static_box_test PRIVATE
    YarsVulkan
    YarsConfiguration
    YarsDataTypes
    YarsUtil
    YarsViewConsole
)

add_dependencies(vulkan_static_box_test compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_static_box_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_static_box_test>/shaders
)

# VulkanHandler test executable
add_executable(vulkan_handler_test
    VulkanHandlerTest.cpp
)

target_link_libraries(vulkan_handler_test PRIVATE
    YarsVulkan
    YarsConfiguration
    YarsDataTypes
    YarsUtil
    YarsViewConsole
)

add_dependencies(vulkan_handler_test compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_handler_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_handler_test>/shaders
)

# Vulkan frame export test executable
add_executable(vulkan_frame_export_test
    VulkanFrameExportTest.cpp
)

target_link_libraries(vulkan_frame_export_test PRIVATE
    YarsVulkan
    YarsConfiguration
    YarsDataTypes
    YarsUtil
    YarsViewConsole
)

add_dependencies(vulkan_frame_export_test compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_frame_export_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_frame_export_test>/shaders
)

# Vulkan Braitenberg capture test executable
add_executable(vulkan_braitenberg_capture
    VulkanBraitenbergCapture.cpp
)

target_link_libraries(vulkan_braitenberg_capture PRIVATE
    YarsVulkan
)

add_dependencies(vulkan_braitenberg_capture compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_braitenberg_capture POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_braitenberg_capture>/shaders
)

# Vulkan Braitenberg scene export executable
add_executable(vulkan_braitenberg_export
    VulkanBraitenbergExport.cpp
)

target_link_libraries(vulkan_braitenberg_export PRIVATE
    YarsVulkan
)

add_dependencies(vulkan_braitenberg_export compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_braitenberg_export POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_braitenberg_export>/shaders
)

# Vulkan ground plane test executable
add_executable(vulkan_ground_plane_test
    VulkanGroundPlaneTest.cpp
)

target_link_libraries(vulkan_ground_plane_test PRIVATE
    YarsVulkan
)

add_dependencies(vulkan_ground_plane_test compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_ground_plane_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_ground_plane_test>/shaders
)

# Vulkan camera control test executable
add_executable(vulkan_camera_control_test
    VulkanCameraControlTest.cpp
)

target_link_libraries(vulkan_camera_control_test PRIVATE
    YarsVulkan
)

add_dependencies(vulkan_camera_control_test compile_shaders)

# Vulkan YARS frame capture executable
add_executable(vulkan_yars_frame_capture
    VulkanYarsFrameCapture.cpp
)

target_link_libraries(vulkan_yars_frame_capture PRIVATE
    YarsVulkan
    YarsConfiguration
    YarsDataTypes
    YarsUtil
    YarsMainControl
)

add_dependencies(vulkan_yars_frame_capture compile_shaders)

# Make sure shaders are available at runtime
add_custom_command(TARGET vulkan_yars_frame_capture POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/compiled
        $<TARGET_FILE_DIR:vulkan_yars_frame_capture>/shaders
)

# Shader compilation
find_program(GLSLC glslc HINTS /opt/homebrew/bin)
find_program(GLSLANGVALIDATOR glslangValidator HINTS /opt/homebrew/bin)

if(GLSLC)
    set(SHADER_COMPILER ${GLSLC})
    set(COMPILER_ARGS "")
elseif(GLSLANGVALIDATOR)
    set(SHADER_COMPILER ${GLSLANGVALIDATOR})
    set(COMPILER_ARGS "-V")  # Vulkan mode for glslangValidator
else()
    message(FATAL_ERROR "Neither glslc nor glslangValidator found - install glslang")
endif()

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/compiled)

file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

# Compile basic shaders
add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/basic_vert.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/basic.vert -o ${SHADER_BINARY_DIR}/basic_vert.spv
    DEPENDS ${SHADER_SOURCE_DIR}/basic.vert
    COMMENT "Compiling basic.vert with ${SHADER_COMPILER}"
)

add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/basic_frag.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/basic.frag -o ${SHADER_BINARY_DIR}/basic_frag.spv
    DEPENDS ${SHADER_SOURCE_DIR}/basic.frag
    COMMENT "Compiling basic.frag with ${SHADER_COMPILER}"
)

# Compile triangle shaders (for testing)
add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/triangle_vert.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/triangle.vert -o ${SHADER_BINARY_DIR}/triangle_vert.spv
    DEPENDS ${SHADER_SOURCE_DIR}/triangle.vert
    COMMENT "Compiling triangle.vert with ${SHADER_COMPILER}"
)

add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/triangle_frag.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/triangle.frag -o ${SHADER_BINARY_DIR}/triangle_frag.spv
    DEPENDS ${SHADER_SOURCE_DIR}/triangle.frag
    COMMENT "Compiling triangle.frag with ${SHADER_COMPILER}"
)

# Compile mesh shaders (with vertex input)
add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/mesh_vert.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/mesh.vert -o ${SHADER_BINARY_DIR}/mesh_vert.spv
    DEPENDS ${SHADER_SOURCE_DIR}/mesh.vert
    COMMENT "Compiling mesh.vert with ${SHADER_COMPILER}"
)

add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/mesh_frag.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/mesh.frag -o ${SHADER_BINARY_DIR}/mesh_frag.spv
    DEPENDS ${SHADER_SOURCE_DIR}/mesh.frag
    COMMENT "Compiling mesh.frag with ${SHADER_COMPILER}"
)

# Compile textured mesh shaders
add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/mesh_textured_vert.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/mesh_textured.vert -o ${SHADER_BINARY_DIR}/mesh_textured_vert.spv
    DEPENDS ${SHADER_SOURCE_DIR}/mesh_textured.vert
    COMMENT "Compiling mesh_textured.vert with ${SHADER_COMPILER}"
)

add_custom_command(
    OUTPUT ${SHADER_BINARY_DIR}/mesh_textured_frag.spv
    COMMAND ${SHADER_COMPILER} ${COMPILER_ARGS} ${SHADER_SOURCE_DIR}/mesh_textured.frag -o ${SHADER_BINARY_DIR}/mesh_textured_frag.spv
    DEPENDS ${SHADER_SOURCE_DIR}/mesh_textured.frag
    COMMENT "Compiling mesh_textured.frag with ${SHADER_COMPILER}"
)

add_custom_target(compile_shaders ALL
    DEPENDS
        ${SHADER_BINARY_DIR}/basic_vert.spv
        ${SHADER_BINARY_DIR}/basic_frag.spv
        ${SHADER_BINARY_DIR}/triangle_vert.spv
        ${SHADER_BINARY_DIR}/triangle_frag.spv
        ${SHADER_BINARY_DIR}/mesh_vert.spv
        ${SHADER_BINARY_DIR}/mesh_frag.spv
        ${SHADER_BINARY_DIR}/mesh_textured_vert.spv
        ${SHADER_BINARY_DIR}/mesh_textured_frag.spv
)

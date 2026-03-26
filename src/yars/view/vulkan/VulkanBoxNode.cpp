#include "VulkanBoxNode.h"
#include "VulkanTextureManager.h"

namespace yars {

VulkanBoxNode::VulkanBoxNode(DataBox* box, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(box)
{
    createGeometry();
    loadTexture();  // Load texture based on material
    update();  // Set initial pose
}

void VulkanBoxNode::createGeometry() {
    // Get dimensions from DataBox
    WHDDimension d = _data->dimension();

    // Use GeometryGenerator to create box with correct dimensions
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Generate box geometry with width=x, depth=y, height=z
    // GeometryGenerator creates unit cube, so we scale by dimensions
    float width = d.width;
    float depth = d.depth;
    float height = d.height;

    // Generate unit cube first
    GeometryGenerator::generateCube(1.0f, vertices, indices);

    // Scale vertices to match DataBox dimensions
    // GeometryGenerator uses Y-up coordinate system, so:
    // X = width, Y = height (vertical), Z = depth

    // Scale vertices to match DataBox dimensions (use XML-specified dimensions exactly)
    for (auto& vertex : vertices) {
        vertex.position.x *= width;
        vertex.position.y *= height;
        vertex.position.z *= depth;
    }

    // Create vertex and index buffers
    if (!_buffer->createVertexBuffer(vertices)) {
        std::cerr << "VulkanBoxNode: Failed to create vertex buffer for box '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    if (!_buffer->createIndexBuffer(indices)) {
        std::cerr << "VulkanBoxNode: Failed to create index buffer for box '"
                  << _data->name() << "'" << std::endl;
        return;
    }
}

void VulkanBoxNode::update() {
    // Update transformation matrix from physics data
    setPose(_data);

    // TEST: Y-offset removed - checking if this was causing visibility issue
    // if (_debugYOffset != 0.0f) {
    //     _modelMatrix[3][1] += _debugYOffset;
    // }
}

void VulkanBoxNode::loadTexture() {
    // TEST: Load texture from DataBox's visualisation data
    // DataBox stores 6 textures (one per face): first, second, third, fourth, fifth, sixth
    // Try loading texture[1] (second) which is GREEN for wall 2, BLUE for others
    std::string materialName = _data->texture(1);  // TEST: Load "second" texture

    if (materialName.empty()) {
        // Fallback to first texture
        materialName = _data->texture(0);
    }

    if (materialName.empty()) {
        std::cerr << "VulkanBoxNode: No texture specified for box '" << _data->name() << "'" << std::endl;
        return;
    }

    std::cout << "TEST: Loading texture[1] (second) for '" << _data->name() << "': " << materialName << std::endl;

    // Load texture via TextureManager
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    if (texManager) {
        VulkanTexture* texture = texManager->getTextureByMaterial(materialName);
        if (texture && texture->isValid()) {
            setTexture(texture);
            setTextureName(materialName);
            std::cout << "VulkanBoxNode: Loaded texture '" << materialName << "' for '" << _data->name() << "'" << std::endl;
        }
    }
}

} // namespace yars

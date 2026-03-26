// Rendering pipeline implementation for VulkanContext
// This file contains render pass, pipeline, command buffers, and drawing code

#include "VulkanContext.h"
#include "VulkanBuffers.h"
#include "VulkanCamera.h"
#include "VulkanSceneGraph.h"
#include "VulkanSceneNode.h"
#include "VulkanEnvironmentNode.h"
#include "VulkanRobotNode.h"
#include "VulkanTraceLineObject.h"
#include <fstream>
#include <iostream>
#include <array>

namespace yars {

// ===== Helper: Read shader file =====
std::vector<char> VulkanContext::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

// ===== Helper: Create shader module =====
VkShaderModule VulkanContext::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
}

// ===== Create Render Pass =====
bool VulkanContext::createRenderPass() {
    // Color attachment
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Depth attachment
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = _depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        std::cerr << "Failed to create render pass!" << std::endl;
        return false;
    }

    return true;
}

// ===== Create Graphics Pipeline =====
bool VulkanContext::createGraphicsPipeline() {
    // Load shaders (using textured mesh shaders that support MVP matrices and textures)
    auto vertShaderCode = readFile("shaders/mesh_textured_vert.spv");
    auto fragShaderCode = readFile("shaders/mesh_textured_frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // Shader stages
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // Vertex input (empty for now - triangle is hardcoded in shader)
    // Vertex input (using Vertex structure from VulkanBuffers.h)
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport and scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_swapchainExtent.width;
    viewport.height = (float)_swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;  // Disable culling to debug visibility issues
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // Standard winding order
    rasterizer.depthBiasEnable = VK_FALSE;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Depth stencil state
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        std::cerr << "Failed to create pipeline layout!" << std::endl;
        vkDestroyShaderModule(_device, fragShaderModule, nullptr);
        vkDestroyShaderModule(_device, vertShaderModule, nullptr);
        return false;
    }

    // Create graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
        std::cerr << "Failed to create graphics pipeline!" << std::endl;
        vkDestroyShaderModule(_device, fragShaderModule, nullptr);
        vkDestroyShaderModule(_device, vertShaderModule, nullptr);
        return false;
    }

    // Clean up shader modules
    vkDestroyShaderModule(_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(_device, vertShaderModule, nullptr);

    return true;
}

// ===== Create Framebuffers =====
bool VulkanContext::createFramebuffers() {
    _swapchainFramebuffers.resize(_swapchainImageViews.size());

    for (size_t i = 0; i < _swapchainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            _swapchainImageViews[i],
            _depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapchainExtent.width;
        framebufferInfo.height = _swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapchainFramebuffers[i]) != VK_SUCCESS) {
            std::cerr << "Failed to create framebuffer!" << std::endl;
            return false;
        }
    }

    return true;
}

// ===== Create Command Pool =====
bool VulkanContext::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = _queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        std::cerr << "Failed to create command pool!" << std::endl;
        return false;
    }

    return true;
}

// ===== Create Command Buffers =====
bool VulkanContext::createCommandBuffers() {
    _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

    if (vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
        std::cerr << "Failed to allocate command buffers!" << std::endl;
        return false;
    }

    return true;
}

// ===== Create Synchronization Objects =====
bool VulkanContext::createSyncObjects() {
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {
            std::cerr << "Failed to create synchronization objects!" << std::endl;
            return false;
        }
    }

    return true;
}

// ===== Draw Frame =====
void VulkanContext::drawFrame(VulkanSceneGraph* sceneGraph) {
    static uint64_t frameCount = 0;
    static bool debugFrameOnce = false;
    if (!debugFrameOnce || (frameCount % 100 == 0)) {
        std::cout << "DEBUG: drawFrame called, frame " << frameCount << std::endl;
        debugFrameOnce = true;
    }
    frameCount++;

    // Update scene graph positions from physics simulation
    if (sceneGraph) {
        sceneGraph->update();
    }

    // Wait for previous frame
    vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire next image
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX,
                                           _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image!");
    }

    vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

    // Record command buffer
    vkResetCommandBuffer(_commandBuffers[_currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(_commandBuffers[_currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass;
    renderPassInfo.framebuffer = _swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapchainExtent;

    // Sky blue background (matching braitenberg scene)
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.53f, 0.81f, 0.92f, 1.0f}};  // Light blue sky
    clearValues[1].depthStencil = {1.0f, 0};  // Depth clear value (far plane)
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(_commandBuffers[_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind pipeline
    vkCmdBindPipeline(_commandBuffers[_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

    // Set up camera matrices using VulkanContext camera state
    static bool debugCameraOnce = false;
    if (!debugCameraOnce) {
        std::cout << "DEBUG: Camera position: (" << _cameraPosition.x << ", " << _cameraPosition.y << ", " << _cameraPosition.z << ")" << std::endl;
        std::cout << "DEBUG: Camera target: (" << _cameraTarget.x << ", " << _cameraTarget.y << ", " << _cameraTarget.z << ")" << std::endl;
        debugCameraOnce = true;
    }

    Camera camera;
    // Use camera from VulkanContext state
    camera.setPosition(_cameraPosition);
    camera.setTarget(_cameraTarget);
    camera.setUp(_cameraUp);

    float aspectRatio = _swapchainExtent.width / (float)_swapchainExtent.height;
    camera.setPerspective(70.0f, aspectRatio, 0.1f, 200.0f);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();

    // Draw all scene objects
    if (sceneGraph) {
        // Render environment objects
        VulkanEnvironmentNode* env = sceneGraph->getEnvironment();
        if (env) {
            // Render ground plane first (as background)
            VulkanSceneNode* groundPlane = env->getGroundPlane();
            if (groundPlane) {
                static bool debugOnce = false;
                if (!debugOnce) {
                    std::cout << "DEBUG: Ground plane exists, hasGeometry=" << groundPlane->hasGeometry()
                              << ", isVisible=" << groundPlane->isVisible() << std::endl;
                    if (groundPlane->getBuffer()) {
                        std::cout << "DEBUG: Ground plane index count: " << groundPlane->getBuffer()->getIndexCount() << std::endl;
                    }
                    debugOnce = true;
                }
                if (groundPlane->hasGeometry() && groundPlane->isVisible()) {
                    renderSceneNode(groundPlane, _commandBuffers[_currentFrame], _currentFrame, view, proj);
                }
            }

            // Render environment objects (walls, etc.)
            const auto& objects = env->getObjects();
            static bool debugObjectsOnce = false;
            static int objectRenderAttempts = 0;
            if (!debugObjectsOnce) {
                std::cout << "DEBUG: Environment has " << objects.size() << " objects" << std::endl;
                debugObjectsOnce = true;
            }
            for (size_t i = 0; i < objects.size(); i++) {
                auto* object = objects[i];
                if (objectRenderAttempts < 20) {
                    std::cout << "DEBUG: Object " << i << ": ptr=" << (void*)object
                              << " hasGeometry=" << (object ? object->hasGeometry() : false)
                              << " isVisible=" << (object ? object->isVisible() : false)
                              << " hasDescriptorSets=" << (object ? object->hasDescriptorSets() : false);
                    if (object) {
                        glm::mat4 model = object->getModelMatrix();
                        glm::vec4 pos = model * glm::vec4(0,0,0,1);
                        std::cout << " pos=(" << pos.x << "," << pos.y << "," << pos.z << ")";
                    }
                    std::cout << std::endl;
                    objectRenderAttempts++;
                }
                if (object && object->hasGeometry() && object->isVisible()) {
                    renderSceneNode(object, _commandBuffers[_currentFrame], _currentFrame, view, proj);
                }
            }
        }

        // Render robots
        static bool debugRobotsOnce = false;
        const auto& robots = sceneGraph->getRobots();
        if (!debugRobotsOnce) {
            std::cout << "DEBUG: Found " << robots.size() << " robot node(s)" << std::endl;
        }
        for (auto* robot : robots) {
            if (robot) {
                const auto& parts = robot->getObjects();
                if (!debugRobotsOnce) {
                    std::cout << "DEBUG: Robot has " << parts.size() << " part(s)" << std::endl;
                }
                for (auto* part : parts) {
                    if (!debugRobotsOnce) {
                        bool hasGeom = part ? part->hasGeometry() : false;
                        bool vis = part ? part->isVisible() : false;
                        bool hasSets = part ? part->hasDescriptorSets() : false;
                        std::cout << "DEBUG: Robot part - ptr=" << part
                                  << ", hasGeometry=" << hasGeom
                                  << ", isVisible=" << vis
                                  << ", hasDescriptorSets=" << hasSets << std::endl;
                        if (part && hasGeom) {
                            std::cout << "DEBUG: Robot part has " << part->getBuffer()->getIndexCount() << " indices" << std::endl;
                        }
                    }
                    if (part && part->hasGeometry() && part->isVisible()) {
                        renderSceneNode(part, _commandBuffers[_currentFrame], _currentFrame, view, proj);
                    }
                }
            }
        }
        if (!debugRobotsOnce) {
            debugRobotsOnce = true;
        }

        // Render trace lines
        for (auto* trace : sceneGraph->getTraceLines()) {
            if (trace && trace->hasGeometry() && trace->isVisible()) {
                renderSceneNode(trace, _commandBuffers[_currentFrame], _currentFrame, view, proj);
            }
        }
    } else {
        // Fallback for backward compatibility: use old single-object rendering
        if (_activeVertexBuffer != VK_NULL_HANDLE && _activeIndexBuffer != VK_NULL_HANDLE) {
            // Bind descriptor sets (for MVP matrices)
            vkCmdBindDescriptorSets(_commandBuffers[_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                   _pipelineLayout, 0, 1, &_descriptorSets[_currentFrame], 0, nullptr);

            // Bind vertex buffer
            VkBuffer vertexBuffers[] = {_activeVertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(_commandBuffers[_currentFrame], 0, 1, vertexBuffers, offsets);

            // Bind index buffer
            vkCmdBindIndexBuffer(_commandBuffers[_currentFrame], _activeIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

            // Draw indexed geometry
            vkCmdDrawIndexed(_commandBuffers[_currentFrame], _activeIndexCount, 1, 0, 0, 0);
        } else {
            // Fallback: draw hardcoded triangle (3 vertices from shader)
            vkCmdDraw(_commandBuffers[_currentFrame], 3, 1, 0, 0);
        }
    }

    vkCmdEndRenderPass(_commandBuffers[_currentFrame]);

    if (vkEndCommandBuffer(_commandBuffers[_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    // Export frames at specific intervals for analysis
    if (frameCount <= 100 && frameCount % 10 == 0) {
        // Wait for rendering to complete before exporting
        vkQueueWaitIdle(_graphicsQueue);

        std::string filename = "yars_scene_frame_" + std::to_string(frameCount) + ".png";
        std::cout << "Exporting frame " << frameCount << " to " << filename << std::endl;

        if (exportFrame(filename)) {
            std::cout << "Successfully exported frame " << frameCount << std::endl;
        } else {
            std::cerr << "Failed to export frame " << frameCount << std::endl;
        }
    }

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = {_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image!");
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// ===== Render Scene Node Helper =====
void VulkanContext::renderSceneNode(VulkanSceneNode* node, VkCommandBuffer cmd, uint32_t frameIndex,
                                    const glm::mat4& view, const glm::mat4& proj) {
    static bool debugRenderOnce = false;
    static uint32_t renderCallCount = 0;
    renderCallCount++;

    // DEBUG: Log details about each render call
    if (!debugRenderOnce && renderCallCount <= 15) {
        glm::mat4 model = node->getModelMatrix();
        glm::vec4 pos = model * glm::vec4(0,0,0,1);
        std::cout << "DEBUG renderSceneNode #" << renderCallCount
                  << ": pos=(" << pos.x << "," << pos.y << "," << pos.z << ")"
                  << " buffer=" << (void*)node->getBuffer()
                  << " indices=" << (node->getBuffer() ? node->getBuffer()->getIndexCount() : 0)
                  << std::endl;
        if (renderCallCount == 15) debugRenderOnce = true;
    }

    VulkanBuffer* buffer = node->getBuffer();
    if (!buffer) {
        std::cout << "DEBUG: node has no buffer!" << std::endl;
        return;
    }

    // Bind texture if this node has one
    VulkanTexture* texture = node->getTexture();

    // DEBUG: Check texture status
    static bool debugTextureOnce = false;
    if (!debugTextureOnce) {
        if (!texture) {
            std::cout << "DEBUG: Node has NO texture assigned!" << std::endl;
        } else if (!texture->isValid()) {
            std::cout << "DEBUG: Node has texture but it's INVALID!" << std::endl;
        } else {
            std::cout << "DEBUG: Node has VALID texture" << std::endl;
        }
        debugTextureOnce = true;
    }

    // Check if node has descriptor sets allocated
    if (!node->hasDescriptorSets()) {
        std::cerr << "Warning: Node has no descriptor sets allocated!" << std::endl;
        return;
    }

    // Update uniform buffer with node's model matrix
    glm::mat4 model = node->getModelMatrix();
    updateUniformBuffer(frameIndex, model, view, proj);

    // DEBUG: Print MVP for first few walls
    static int mvpDebugCount = 0;
    if (mvpDebugCount < 5) {
        glm::mat4 mvp = proj * view * model;
        glm::vec4 corners[8] = {
            mvp * glm::vec4(-0.5, -0.5, -0.5, 1.0),  // Near bottom left
            mvp * glm::vec4(0.5, -0.5, -0.5, 1.0),   // Near bottom right
            mvp * glm::vec4(-0.5, 0.5, -0.5, 1.0),   // Near top left
            mvp * glm::vec4(0.5, 0.5, -0.5, 1.0),    // Near top right
            mvp * glm::vec4(-0.5, -0.5, 0.5, 1.0),   // Far bottom left
            mvp * glm::vec4(0.5, -0.5, 0.5, 1.0),    // Far bottom right
            mvp * glm::vec4(-0.5, 0.5, 0.5, 1.0),    // Far top left
            mvp * glm::vec4(0.5, 0.5, 0.5, 1.0)      // Far top right
        };
        std::cout << "DEBUG MVP #" << mvpDebugCount << ": Testing clip space" << std::endl;
        bool anyVisible = false;
        for (int i = 0; i < 8; i++) {
            float x = corners[i].x / corners[i].w;
            float y = corners[i].y / corners[i].w;
            float z = corners[i].z / corners[i].w;
            bool visible = (x >= -1 && x <= 1 && y >= -1 && y <= 1 && z >= 0 && z <= 1);
            if (visible) anyVisible = true;
            if (i < 2) {  // Just print first two
                std::cout << "  Corner " << i << ": NDC(" << x << ", " << y << ", " << z << ") visible=" << visible << std::endl;
            }
        }
        std::cout << "  Any corner visible: " << anyVisible << std::endl;
        mvpDebugCount++;
    }

    // Bind node's descriptor set (already configured with its texture)
    const std::vector<VkDescriptorSet>& nodeSets = node->getDescriptorSets();
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                           _pipelineLayout, 0, 1, &nodeSets[frameIndex], 0, nullptr);

    // Bind vertex buffer
    VkBuffer vertexBuffers[] = {buffer->getVertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

    // Bind index buffer
    vkCmdBindIndexBuffer(cmd, buffer->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    // Draw indexed geometry
    vkCmdDrawIndexed(cmd, buffer->getIndexCount(), 1, 0, 0, 0);
}

// ============================================================================
// Set Active Geometry
// ============================================================================

void VulkanContext::setActiveGeometry(VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t indexCount) {
    _activeVertexBuffer = vertexBuffer;
    _activeIndexBuffer = indexBuffer;
    _activeIndexCount = indexCount;
}

// ============================================================================
// Camera Controls
// ============================================================================

void VulkanContext::handleMouseMotion(int dx, int dy, bool leftButton, bool rightButton) {
    if (leftButton) {
        // Left mouse button: Orbit camera around target
        _cameraYaw -= dx * _mouseSensitivity;
        _cameraPitch -= dy * _mouseSensitivity;

        // Clamp pitch to prevent camera flipping
        if (_cameraPitch > 89.0f) _cameraPitch = 89.0f;
        if (_cameraPitch < -89.0f) _cameraPitch = -89.0f;

        // Convert spherical coordinates to Cartesian
        float yawRad = glm::radians(_cameraYaw);
        float pitchRad = glm::radians(_cameraPitch);

        _cameraPosition.x = _cameraTarget.x + _cameraDistance * cos(pitchRad) * cos(yawRad);
        _cameraPosition.y = _cameraTarget.y + _cameraDistance * sin(pitchRad);
        _cameraPosition.z = _cameraTarget.z + _cameraDistance * cos(pitchRad) * sin(yawRad);
    }

    if (rightButton) {
        // Right mouse button: Zoom camera (adjust distance)
        _cameraDistance += dy * 0.1f;

        // Clamp distance to reasonable values
        if (_cameraDistance < 1.0f) _cameraDistance = 1.0f;
        if (_cameraDistance > 100.0f) _cameraDistance = 100.0f;

        // Update camera position with new distance
        float yawRad = glm::radians(_cameraYaw);
        float pitchRad = glm::radians(_cameraPitch);

        _cameraPosition.x = _cameraTarget.x + _cameraDistance * cos(pitchRad) * cos(yawRad);
        _cameraPosition.y = _cameraTarget.y + _cameraDistance * sin(pitchRad);
        _cameraPosition.z = _cameraTarget.z + _cameraDistance * cos(pitchRad) * sin(yawRad);
    }
}

void VulkanContext::handleKeyboard(const bool* keys, float deltaTime) {
    // Calculate camera basis vectors for movement
    glm::vec3 forward = glm::normalize(_cameraTarget - _cameraPosition);
    glm::vec3 right = glm::normalize(glm::cross(forward, _cameraUp));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    float speed = _cameraSpeed * deltaTime;

    // WASD for horizontal movement
    // SDL scancodes: W=26, A=4, S=22, D=7
    if (keys[26]) {  // W - move forward
        _cameraPosition += forward * speed;
        _cameraTarget += forward * speed;
    }
    if (keys[22]) {  // S - move backward
        _cameraPosition -= forward * speed;
        _cameraTarget -= forward * speed;
    }
    if (keys[4]) {   // A - move left
        _cameraPosition -= right * speed;
        _cameraTarget -= right * speed;
    }
    if (keys[7]) {   // D - move right
        _cameraPosition += right * speed;
        _cameraTarget += right * speed;
    }

    // Q/E for vertical movement
    // SDL scancodes: Q=20, E=8
    if (keys[20]) {  // Q - move down
        _cameraPosition -= up * speed;
        _cameraTarget -= up * speed;
    }
    if (keys[8]) {   // E - move up
        _cameraPosition += up * speed;
        _cameraTarget += up * speed;
    }

    // Arrow keys for rotation
    // SDL scancodes: Up=82, Down=81, Left=80, Right=79
    if (keys[79]) {  // Right arrow - rotate right
        _cameraYaw += 90.0f * deltaTime;
        float yawRad = glm::radians(_cameraYaw);
        float pitchRad = glm::radians(_cameraPitch);
        _cameraPosition.x = _cameraTarget.x + _cameraDistance * cos(pitchRad) * cos(yawRad);
        _cameraPosition.z = _cameraTarget.z + _cameraDistance * cos(pitchRad) * sin(yawRad);
    }
    if (keys[80]) {  // Left arrow - rotate left
        _cameraYaw -= 90.0f * deltaTime;
        float yawRad = glm::radians(_cameraYaw);
        float pitchRad = glm::radians(_cameraPitch);
        _cameraPosition.x = _cameraTarget.x + _cameraDistance * cos(pitchRad) * cos(yawRad);
        _cameraPosition.z = _cameraTarget.z + _cameraDistance * cos(pitchRad) * sin(yawRad);
    }
    if (keys[82]) {  // Up arrow - rotate up
        _cameraPitch += 45.0f * deltaTime;
        if (_cameraPitch > 89.0f) _cameraPitch = 89.0f;
        float pitchRad = glm::radians(_cameraPitch);
        _cameraPosition.y = _cameraTarget.y + _cameraDistance * sin(pitchRad);
    }
    if (keys[81]) {  // Down arrow - rotate down
        _cameraPitch -= 45.0f * deltaTime;
        if (_cameraPitch < -89.0f) _cameraPitch = -89.0f;
        float pitchRad = glm::radians(_cameraPitch);
        _cameraPosition.y = _cameraTarget.y + _cameraDistance * sin(pitchRad);
    }
}

void VulkanContext::resetCamera() {
    // Oblique view to see walls from an angle (not straight top-down)
    _cameraPosition = glm::vec3(8.0f, 8.0f, 8.0f);
    _cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    _cameraDistance = glm::length(_cameraPosition - _cameraTarget);
    _cameraYaw = 0.0f;
    _cameraPitch = -45.0f;
}

void VulkanContext::setCameraPosition(const glm::vec3& position, const glm::vec3& target) {
    _cameraPosition = position;
    _cameraTarget = target;

    // Calculate distance, yaw, and pitch from position and target
    glm::vec3 delta = _cameraPosition - _cameraTarget;
    _cameraDistance = glm::length(delta);

    if (_cameraDistance > 0.001f) {
        _cameraYaw = glm::degrees(atan2(delta.z, delta.x));
        _cameraPitch = glm::degrees(asin(delta.y / _cameraDistance));
    }
}

void VulkanContext::setCameraFromYARS(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& up) {
    _cameraPosition = position;
    _cameraTarget = lookAt;
    _cameraUp = up;

    // Calculate distance, yaw, and pitch from position and target
    glm::vec3 delta = _cameraPosition - _cameraTarget;
    _cameraDistance = glm::length(delta);

    if (_cameraDistance > 0.001f) {
        _cameraYaw = glm::degrees(atan2(delta.z, delta.x));
        _cameraPitch = glm::degrees(asin(delta.y / _cameraDistance));
    }

    std::cout << "VulkanContext: Camera set from YARS configuration:" << std::endl;
    std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    std::cout << "  LookAt: (" << lookAt.x << ", " << lookAt.y << ", " << lookAt.z << ")" << std::endl;
    std::cout << "  Up: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
}

} // namespace yars

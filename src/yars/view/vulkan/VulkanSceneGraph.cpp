#include "VulkanSceneGraph.h"
#include "VulkanRobotNode.h"
#include "VulkanEnvironmentNode.h"
#include "VulkanTraceLineObject.h"
#include <yars/configuration/data/Data.h>
#include <yars/configuration/data/DataRobot.h>
#include <yars/configuration/data/DataEnvironment.h>
#include <yars/configuration/data/DataTraces.h>
#include <yars/util/macros.h>
#include <iostream>

namespace yars {

VulkanSceneGraph::VulkanSceneGraph(VulkanContext* context)
    : _context(context)
    , _environment(nullptr)
{
    std::cout << "VulkanSceneGraph: Initializing..." << std::endl;

    // Create scene nodes from YARS data
    createEnvironmentNode();
    createRobotNodes();
    createTraces();

    std::cout << "VulkanSceneGraph: Created " << _robots.size() << " robot nodes" << std::endl;
}

VulkanSceneGraph::~VulkanSceneGraph() {
    std::cout << "VulkanSceneGraph: Cleaning up..." << std::endl;

    // Cleanup robot nodes
    for (auto* robot : _robots) {
        delete robot;
    }
    _robots.clear();

    // Cleanup environment
    if (_environment) {
        delete _environment;
        _environment = nullptr;
    }

    // Cleanup trace lines
    for (auto* trace : _traceLines) {
        delete trace;
    }
    _traceLines.clear();
}

void VulkanSceneGraph::update() {
    // Update all robot nodes from physics data
    for (auto* robot : _robots) {
        if (robot) {
            robot->update();
        }
    }

    // Update environment
    if (_environment) {
        _environment->update();
    }

    // Update trace lines
    for (auto* trace : _traceLines) {
        if (trace) {
            trace->update();
        }
    }
}

void VulkanSceneGraph::reset() {
    std::cout << "VulkanSceneGraph: Resetting scene..." << std::endl;

    // Reset all robot nodes
    for (auto* robot : _robots) {
        if (robot) {
            robot->reset();
        }
    }

    // Reset environment
    if (_environment) {
        _environment->reset();
    }
}

void VulkanSceneGraph::createRobotNodes() {
    // Read robots from YARS Data
    // Pattern from SceneGraph.cpp:75-81
    DataRobots* robots = Data::instance()->current()->robots();

    std::cout << "VulkanSceneGraph: Creating robot nodes..." << std::endl;

    FOREACHP(DataRobot*, robot, robots) {
        VulkanRobotNode* robotNode = new VulkanRobotNode(*robot, _context);
        _robots.push_back(robotNode);
    }

    std::cout << "VulkanSceneGraph: Created " << _robots.size() << " robot(s)" << std::endl;
}

void VulkanSceneGraph::createEnvironmentNode() {
    // Read environment from YARS Data
    // Pattern from SceneGraph.cpp:84-88
    DataEnvironment* environment = Data::instance()->current()->environment();

    std::cout << "VulkanSceneGraph: Creating environment node..." << std::endl;

    if (environment) {
        _environment = new VulkanEnvironmentNode(environment, _context);
        std::cout << "VulkanSceneGraph: Environment created with "
                  << _environment->getObjectCount() << " objects" << std::endl;
    } else {
        std::cout << "VulkanSceneGraph: No environment data" << std::endl;
    }
}

void VulkanSceneGraph::createTraces() {
    // Read traces from YARS Data
    // Pattern from SceneGraph.cpp:101-122
    DataTraces* traces = Data::instance()->current()->traces();

    std::cout << "VulkanSceneGraph: Creating trace objects..." << std::endl;

    if (!traces) {
        std::cout << "VulkanSceneGraph: No trace data" << std::endl;
        return;
    }

    // Create trace line objects
    for (auto t = traces->l_begin(); t != traces->l_end(); t++) {
        VulkanTraceLineObject* trace = new VulkanTraceLineObject(*t, _context);
        _traceLines.push_back(trace);
    }

    std::cout << "VulkanSceneGraph: Created " << _traceLines.size() << " trace line object(s)" << std::endl;
}

} // namespace yars

#include "NativeOgreWindow.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/util/macros.h>

#include <OGRE/Ogre.h>
#include <sstream>

using namespace std;

NativeOgreWindow::NativeOgreWindow(int index)
    : _visible(false),
      _added(false),
      _closed(false)
{
    _index = index;
    _windowConfiguration = new WindowConfiguration(index);
    _data = Data::instance()->current()->screens()->screen(index);
    _camData = Data::instance()->current()->screens()->screen(index)->camera();

    __setupNativeWindow();
}

NativeOgreWindow::~NativeOgreWindow()
{
    // Cleanup handled in close()
}

void NativeOgreWindow::__setupNativeWindow()
{
    _ogreHandler = OgreHandler::instance();

    // Create native OGRE window (no external context)
    stringstream oss;
    oss << "YARS Native Window " << _index;

    Ogre::NameValuePairList params;
    // Use native windowing - no external context
    params["vsync"] = "false";

    _window = _ogreHandler->root()->createRenderWindow(
        oss.str(),
        _windowConfiguration->geometry.width(),
        _windowConfiguration->geometry.height(),
        false, // not fullscreen
        &params);

    _window->setActive(true);
    _window->setVisible(true);

    Ogre::Vector3 pos;
    Ogre::Vector3 lookAt;

    YARS_TO_OGRE(_windowConfiguration->cameraPosition, pos);
    YARS_TO_OGRE(_windowConfiguration->cameraLookAt, lookAt);

    _sceneManager = _ogreHandler->getSceneManager();

    // Create camera
    oss.str("");
    oss << "YARS Native Camera" << _index;
    _camera = _sceneManager->createCamera(oss.str());
    _camera->setNearClipDistance(0.01f);
    _camera->setFarClipDistance(1000000.0f);

    // Create camera scene node and attach camera to it
    oss.str("");
    oss << "YARS Native CameraNode" << _index;
    _cameraNode = _sceneManager->getRootSceneNode()->createChildSceneNode(oss.str());
    _cameraNode->attachObject(_camera);

    // Set initial position and orientation using scene node
    _cameraNode->setPosition(pos[0], pos[1], pos[2]);
    _cameraNode->lookAt(Ogre::Vector3(lookAt[0], lookAt[1], lookAt[2]), Ogre::Node::TS_WORLD);

    const Ogre::Real aspectRatio = Ogre::Real(_windowConfiguration->geometry.width()) / Ogre::Real(_windowConfiguration->geometry.height());
    _camera->setAspectRatio(aspectRatio);

    _viewport = _window->addViewport(_camera);
    Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
    _viewport->setBackgroundColour(fadeColour);

    // Use default material scheme (fixed-function pipeline) since RTSS is disabled
    _viewport->setMaterialScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
    std::cout << "Native viewport configured to use default material scheme (fixed-function)" << std::endl;

    _window->setActive(true);
    _viewport->update();
    _visible = true;
}

void NativeOgreWindow::handleEvents()
{
    // Native OGRE window events are handled automatically
    // Check for window close events
    if (_window && _window->isClosed())
    {
        _closed = true;
    }
}

void NativeOgreWindow::step()
{
    handleEvents();

    if (_closed)
    {
        return;
    }

    // Simple render step
    if (_window && _window->isActive())
    {
        _window->update();
    }
}

void NativeOgreWindow::setupOSD()
{
    // Simplified - no overlay system for stability testing
    std::cout << "OSD setup skipped for stability testing" << std::endl;
}

void NativeOgreWindow::reset()
{
    // Nothing to reset in simplified version
}

void NativeOgreWindow::quit()
{
    close();
}

bool NativeOgreWindow::visible()
{
    return _visible && !_closed;
}

bool NativeOgreWindow::added()
{
    return _added;
}

void NativeOgreWindow::setAdded()
{
    _added = true;
}

bool NativeOgreWindow::closed()
{
    return _closed;
}

void NativeOgreWindow::close()
{
    if (_window)
    {
        _window->destroy();
        _window = nullptr;
    }
    _closed = true;
}
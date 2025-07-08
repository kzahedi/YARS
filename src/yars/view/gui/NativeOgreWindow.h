#ifndef __NATIVE_OGRE_WINDOW_H__
#define __NATIVE_OGRE_WINDOW_H__

#include <yars/util/Observable.h>
#include <yars/util/ObservableMessage.h>
#include <yars/configuration/data/Data.h>
#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/OgreHandler.h>

#include <OGRE/Ogre.h>

class NativeOgreWindow : public Observable
{
public:
    NativeOgreWindow(int index);
    ~NativeOgreWindow();

    void reset();
    void quit();
    void step();
    void handleEvents();
    bool visible();
    bool added();
    void setAdded();
    bool closed();
    void close();
    void setupOSD();

private:
    void __setupNativeWindow();

    Ogre::RenderWindow *_window;
    Ogre::Camera *_camera;
    Ogre::SceneNode *_cameraNode;
    Ogre::Viewport *_viewport;
    Ogre::SceneManager *_sceneManager;

    DataScreen *_data;
    DataCamera *_camData;
    WindowConfiguration *_windowConfiguration;
    OgreHandler *_ogreHandler;

    int _index;
    bool _visible;
    bool _added;
    bool _closed;
};

#endif // __NATIVE_OGRE_WINDOW_H__
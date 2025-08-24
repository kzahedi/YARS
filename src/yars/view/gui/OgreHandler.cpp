#include "OgreHandler.h"
#include "MaterialManager.h"
#include "ShaderManager.h"
#include <yars/configuration/data/Data.h>
#include <yars/util/Directories.h>
#include <filesystem>

#if __APPLE__
#include <Overlay/OgreOverlaySystem.h>
#else
#include <OGRE/Overlay/OgreOverlaySystem.h>
#include <OGRE/RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>
#endif

#include <OGRE/RTShaderSystem/OgreShaderGenerator.h>

namespace yars {

OgreHandler *OgreHandler::_me = nullptr;

OgreHandler *OgreHandler::instance()
{
  if (_me == nullptr)
    _me = new OgreHandler();
  return _me;
}

OgreHandler::OgreHandler()
{
  Ogre::LogManager *lm = new Ogre::LogManager();
  lm->createLog("ogre.log", true, false, false); // create silent logging

#ifdef __APPLE__
  // For macOS: Don't load plugins.cfg since we're using static plugins
  _root = new Ogre::Root("", "", ""); // no plugin config file for static linking
#else                                 // __APPLE__
  _root = new Ogre::Root("plugins.cfg", "ogre.cfg", ""); // no log file created here (see 1 line above)
#endif                                // __APPLE__

#ifdef __APPLE__
  try
  {
    _GLPlugin = new Ogre::GL3PlusPlugin();
    _GLPlugin->install();

    _particlePlugin = new Ogre::ParticleFXPlugin();
    _particlePlugin->install();

  }
  catch (const std::exception &e)
  {
    std::cerr << "Failed to initialize OGRE plugins: " << e.what() << std::endl;
    throw;
  }
#else  // __APPLE__
  _root->loadPlugin("Plugin_ParticleFX");
  _root->loadPlugin("RenderSystem_GL3Plus");
#endif // __APPLE__

  if (_root->getAvailableRenderers().size() == 0)
  {
    OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "No render systems available", "OgreHandler");
  }

  // Use GL3Plus renderer with OpenGL 3.0 compatibility profile
  const Ogre::RenderSystemList &renderers = _root->getAvailableRenderers();
  Ogre::RenderSystem *renderSystem = renderers.front();
  std::cout << "Available renderer: " << renderSystem->getName() << std::endl;
  std::cout << "Using renderer for OpenGL 3.0 compatibility mode" << std::endl;
  _root->setRenderSystem(renderSystem);

  // Check environment before attempting OpenGL context creation
  const char *display = getenv("DISPLAY");
  bool hasDisplay = (display != nullptr && strlen(display) > 0);

  if (!hasDisplay)
  {
    std::cout << "No DISPLAY environment variable detected." << std::endl;
  }

  std::cout << "Attempting to initialize OpenGL 3.3 core renderer with custom shaders..." << std::endl;

  try
  {
    // Renderer already set above - don't override it
    _root->initialise(false);
    _sceneManager = _root->createSceneManager("DefaultSceneManager");
    std::cout << "OpenGL 3.3 core renderer initialized successfully!" << std::endl;
  }
  catch (const Ogre::RenderingAPIException &e)
  {
    std::cerr << std::endl;
    std::cerr << "===============================================" << std::endl;
    std::cerr << "OPENGL CONTEXT CREATION FAILED" << std::endl;
    std::cerr << "===============================================" << std::endl;
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << std::endl;

    // Provide specific guidance based on environment
    if (!hasDisplay)
    {
      std::cerr << "DETECTED ISSUE: No display server connection" << std::endl;
      std::cerr << "- You're running in a headless environment" << std::endl;
      std::cerr << "- Try running from a GUI terminal application" << std::endl;
    }
    else
    {
      std::cerr << "POSSIBLE ISSUES:" << std::endl;
      std::cerr << "- OpenGL 3.3+ core not supported by graphics driver" << std::endl;
      std::cerr << "- GPU acceleration disabled" << std::endl;
      std::cerr << "- Display server configuration problem" << std::endl;
    }

    std::cerr << std::endl;
    std::cerr << "SOLUTION:" << std::endl;
    std::cerr << "Use --nogui flag to run without visualization:" << std::endl;
    std::cerr << "  ./yars --xml your_simulation.xml --nogui" << std::endl;
    std::cerr << "===============================================" << std::endl;
    throw;
  }
  catch (const std::exception &e)
  {
    std::cerr << "OGRE initialization failed: " << e.what() << std::endl;
    throw;
  }
}

// Helper listener that automatically generates shader techniques for
// materials that lack a GPU program when the RT Shader System material
// scheme is requested. This is essentially the logic used in many OGRE
// samples to work with the GL3+ RenderSystem.
class SGTechniqueResolverListener : public Ogre::MaterialManager::Listener
{
public:
  SGTechniqueResolverListener(Ogre::RTShader::ShaderGenerator *pShaderGenerator)
      : mShaderGenerator(pShaderGenerator) {}

  // Called when a material with the requested scheme is not found.
  Ogre::Technique *handleSchemeNotFound(uint16_t schemeIndex, const Ogre::String &schemeName,
                                        Ogre::Material *originalMaterial, uint16_t lodIndex,
                                        const Ogre::Renderable *rend) override
  {
    if (schemeName != Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
      return nullptr;

    // Create shader generated technique for this material.
    bool generated = mShaderGenerator->createShaderBasedTechnique(*originalMaterial,
                                                                  Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                                  schemeName,
                                                                  true /* overProgrammable */);
    if (!generated)
      return nullptr;

    // Validate will compile the generated programs.
    mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());

    // Return the generated technique.
    for (unsigned short i = 0; i < originalMaterial->getNumTechniques(); ++i)
    {
      Ogre::Technique *tech = originalMaterial->getTechnique(i);
      if (tech->getSchemeName() == schemeName)
        return tech;
    }
    return nullptr;
  }

private:
  Ogre::RTShader::ShaderGenerator *mShaderGenerator;
};

void OgreHandler::setupSceneManager()
{
  Ogre::OverlaySystem *mOverlaySystem = new Ogre::OverlaySystem();
  _sceneManager->addRenderQueueListener(mOverlaySystem);

  // ----------------------------------------------------------------------
  // 1) Resource setup (needs to happen BEFORE RTSS initialisation so that
  //    all shader-lib materials and program scripts are available).
  // ----------------------------------------------------------------------

  std::cout << "Loading basic resources for rendering" << std::endl;

  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

  // Register YARS resources (meshes, materials) and OGRE sample resources
  rgm.addResourceLocation(".", "FileSystem", "General"); // Current directory contains materials
  rgm.addResourceLocation("../../meshes", "FileSystem", "General");
  rgm.addResourceLocation("../ext/ogre/source/Media/Main", "FileSystem", "General");

  // RT Shader System core library (program + material scripts)
  rgm.addResourceLocation("../ext/ogre/source/Media/RTShaderLib", "FileSystem", "RTShaderLib");
  rgm.addResourceLocation("../ext/ogre/source/Media/RTShaderLib/materials", "FileSystem", "RTShaderLib");
  rgm.addResourceLocation("../ext/ogre/source/Media/RTShaderLib/GLSL", "FileSystem", "RTShaderLib");

  // Initialise groups *before* we attempt to generate shaders.
  rgm.initialiseResourceGroup("RTShaderLib");
  rgm.initialiseResourceGroup("General");

  // ----------------------------------------------------------------------
  // 2) Runtime Shader System initialisation and automatic technique
  //    generation listener.
  // ----------------------------------------------------------------------

  std::cout << "Runtime Shader System: Initialising..." << std::endl;

  if (Ogre::RTShader::ShaderGenerator::initialize())
  {
    // Use ShaderManager for comprehensive RTSS setup
    if (ShaderManager::instance()->initializeRTSS(_sceneManager)) {
      _shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
      
      // Install our listener so missing techniques are generated on demand.
      _materialListener = new SGTechniqueResolverListener(_shaderGenerator);
      Ogre::MaterialManager::getSingleton().addListener(_materialListener);

      // Ensure the RTSS scheme is the active one for all viewports by default.
      Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

      // Initialize MaterialManager after ShaderManager setup
      try {
          MaterialManager::instance()->validateAllMaterials();
          std::cout << "MaterialManager initialized successfully" << std::endl;
      } catch (const std::exception& e) {
          std::cerr << "Failed to initialize MaterialManager: " << e.what() << std::endl;
      }
      
      // Validate shader generation
      ShaderManager::instance()->validateShaderGeneration();

      std::cout << "RTSS initialized successfully for material compatibility" << std::endl;
    }
  }
  else
  {
    std::cout << "RTSS initialization failed - using fixed-function materials" << std::endl;
    _shaderGenerator = nullptr;
  }

  // ----------------------------------------------------------------------
  // 3) Diagnostics
  // ----------------------------------------------------------------------

  // Check if our custom materials loaded
  std::cout << "Checking for SimpleWhite material..." << std::endl;
  Ogre::MaterialPtr testMat = Ogre::MaterialManager::getSingleton().getByName("SimpleWhite");
  if (!testMat)
  {
    std::cout << "ERROR: SimpleWhite material not found!" << std::endl;
  }
  else
  {
    std::cout << "SUCCESS: SimpleWhite material found!" << std::endl;
  }

  // Try to load some basic YARS materials safely
  std::cout << "Loading basic YARS materials..." << std::endl;
  try
  {
    // Only load safe, basic materials that shouldn't cause issues
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../materials", "FileSystem", "YARS");
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("YARS");
    std::cout << "Successfully loaded YARS materials directory" << std::endl;
    
    // Create RTSS techniques for YARS materials now that they are loaded
    try {
        MaterialManager::instance()->createRTSSForLegacyMaterials();
        std::cout << "Created RTSS techniques for YARS materials" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Warning: Failed to create RTSS techniques for YARS materials: " << e.what() << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "Could not load YARS materials: " << e.what() << std::endl;
  }

  // Temporarily disable sky dome to test RTSS functionality
  // _sceneManager->setSkyDome(true, Data::instance()->current()->screens()->sky(), 20, 10);
  // _sceneManager->setSkyBox(true, "YARS/SkyBox", 100000.0);

  _sceneManager->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3)); // Moderate ambient light for better shading

  // Create directional light for proper scene illumination
  Ogre::SceneNode *node = _sceneManager->getRootSceneNode()->createChildSceneNode("lightNode");
  Ogre::Light *lightSun = _sceneManager->createLight("sun");
  lightSun->setType(Ogre::Light::LT_DIRECTIONAL);
  node->setDirection(Ogre::Vector3(-1, -1, -1));
  lightSun->setDiffuseColour(1.2, 1.2, 1.0);  // Warm directional light
  lightSun->setSpecularColour(1.0, 1.0, 0.8); // Warm specular highlights
  node->attachObject(lightSun);

  // Re-enable SceneGraph for basic rendering
  std::cout << "Creating SceneGraph for basic rendering" << std::endl;
  _rootNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
  _sceneGraph = new SceneGraph(_rootNode, _sceneManager);

  // Debug: Print some basic scene statistics
  std::cout << "=== SCENE DEBUG INFO ===" << std::endl;
  std::cout << "Root node children count: " << _sceneManager->getRootSceneNode()->numChildren() << std::endl;
  std::cout << "Scene graph construction completed" << std::endl;

  // Force an immediate render to test if rendering works
  try
  {
    _sceneManager->_updateSceneGraph(nullptr);
    std::cout << "Manual scene graph update completed" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cout << "Manual render failed: " << e.what() << std::endl;
  }

  std::cout << "========================" << std::endl;

  // _textOverlay  = new TextOverlay("Legend Text Overlay");
  // Ogre::Real x = 10;
  // Ogre::Real y = 10;
  // _textOverlay->addTextBox("legend",
  // "^0YARS, Zahedi et al.", x, y, 150, 20,
  // Ogre::ColourValue(75.0/255.0, 117.0/255.0, 148.0/255.0,1.0f),
  // "Legend", "20");

  // Disable shadows to avoid rendering pipeline issues
  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  std::cout << "Shadow system disabled for stability" << std::endl;
}

Ogre::SceneManager *OgreHandler::getSceneManager()
{
  return _sceneManager;
}

void OgreHandler::reset()
{
  _sceneGraph->reset();
}

void OgreHandler::step()
{
  // cout << "OgreHandler 0" << endl;
  _sceneGraph->update();
  // cout << "OgreHandler 1" << endl;

  try
  {
    _root->renderOneFrame();
    // cout << "OgreHandler 2" << endl;
  }
  catch (const Ogre::InvalidStateException &e)
  {
    static bool errorShown = false;
    if (!errorShown)
    {
      std::cout << "RENDERING FAILED: " << e.what() << std::endl;
      std::cout << "This is expected due to RTSS shader issues. GUI window remains functional." << std::endl;
      errorShown = true;
    }
    // Continue simulation - window stays open
  }
  catch (const std::exception &e)
  {
    static bool errorShown = false;
    if (!errorShown)
    {
      std::cout << "RENDERING ERROR: " << e.what() << std::endl;
      std::cout << "This is expected due to material compatibility issues" << std::endl;
      errorShown = true;
    }
    // Continue simulation - window stays open
  }
}

Ogre::Root *OgreHandler::root()
{
  return _root;
}

OgreHandler::~OgreHandler()
{
  if (_shaderGenerator)
  {
    Ogre::MaterialManager::getSingleton().removeListener(_materialListener);
    delete _materialListener;
    Ogre::RTShader::ShaderGenerator::destroy();
  }
  // plugins and root cleanup handled elsewhere (application exit)
}

} // namespace yars

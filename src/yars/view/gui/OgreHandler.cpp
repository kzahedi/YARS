#include "OgreHandler.h"
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

OgreHandler *OgreHandler::_me = NULL;

OgreHandler *OgreHandler::instance()
{
  if (_me == NULL)
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
  rgm.addResourceLocation("ext/ogre/Media/Main", "FileSystem", "General");

  // RT Shader System core library (program + material scripts)
  rgm.addResourceLocation("ext/ogre/Media/RTShaderLib", "FileSystem", "RTShaderLib");
  rgm.addResourceLocation("ext/ogre/Media/RTShaderLib/materials", "FileSystem", "RTShaderLib");

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
    _shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    _shaderGenerator->addSceneManager(_sceneManager);

    // Set GLSL as the default target language for the GL3+ RenderSystem.
    _shaderGenerator->setTargetLanguage("glsl");

    // Cache generated shaders under a writable directory to avoid recompilation cost.
    const std::string cacheDir = "./rtshadercache";
    std::error_code ec;
    std::filesystem::create_directories(cacheDir, ec); // ignore errors; path may already exist
    _shaderGenerator->setShaderCachePath(cacheDir);

    // Install our listener so missing techniques are generated on demand.
    _materialListener = new SGTechniqueResolverListener(_shaderGenerator);
    Ogre::MaterialManager::getSingleton().addListener(_materialListener);

    // Ensure the RTSS scheme is the active one for all viewports by default.
    Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    // Create a minimal material that RTSS can handle
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("SimpleLit", "General");
    mat->setReceiveShadows(false);
    mat->getTechnique(0)->setLightingEnabled(true);                                       // Enable lighting for proper shading
    mat->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(0.8, 0.1, 0.1, 1.0));  // Red diffuse
    mat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue(0.3, 0.1, 0.1));       // Dark red ambient
    mat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue(1.0, 0.8, 0.8, 1.0)); // White specular highlights
    mat->getTechnique(0)->getPass(0)->setShininess(32.0f);                                // Shiny surface
    std::cout << "Created SimpleLit material for RTSS (red with proper lighting)" << std::endl;

    // Add a simple procedural texture to the sphere
    try
    {
      // Create a radial gradient texture for the sphere
      Ogre::TexturePtr sphereTex = Ogre::TextureManager::getSingleton().createManual(
          "SphereTexture", "General", Ogre::TEX_TYPE_2D, 128, 128, 0, Ogre::PF_R8G8B8A8);

      Ogre::HardwarePixelBufferSharedPtr buffer = sphereTex->getBuffer();
      buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
      const Ogre::PixelBox &pb = buffer->getCurrentLock();
      Ogre::uint8 *data = static_cast<Ogre::uint8 *>(pb.data);

      for (size_t y = 0; y < 128; ++y)
      {
        for (size_t x = 0; x < 128; ++x)
        {
          // Create concentric circles pattern
          float dx = (x - 64.0f) / 64.0f;
          float dy = (y - 64.0f) / 64.0f;
          float dist = sqrt(dx * dx + dy * dy);
          Ogre::uint8 intensity = (Ogre::uint8)(255 * (0.5 + 0.5 * sin(dist * 8.0f)));

          size_t index = (y * 128 + x) * 4;
          data[index + 0] = intensity;     // R
          data[index + 1] = intensity / 4; // G
          data[index + 2] = intensity / 4; // B
          data[index + 3] = 255;           // A
        }
      }
      buffer->unlock();

      // Apply texture to sphere material
      Ogre::TextureUnitState *texUnit = mat->getTechnique(0)->getPass(0)->createTextureUnitState();
      texUnit->setTexture(sphereTex);
      texUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
      std::cout << "Added concentric pattern texture to sphere material" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Failed to create sphere texture: " << e.what() << std::endl;
    }

    // Create ground material with different colors
    Ogre::MaterialPtr groundMat = Ogre::MaterialManager::getSingleton().create("GroundLit", "General");
    groundMat->setReceiveShadows(true); // Ground should receive shadows
    groundMat->getTechnique(0)->setLightingEnabled(true);
    groundMat->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue(0.2, 0.6, 0.2, 1.0));  // Green diffuse
    groundMat->getTechnique(0)->getPass(0)->setAmbient(Ogre::ColourValue(0.1, 0.3, 0.1));       // Dark green ambient
    groundMat->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue(0.4, 0.8, 0.4, 1.0)); // Green specular
    groundMat->getTechnique(0)->getPass(0)->setShininess(16.0f);                                // Less shiny than sphere

    // Add a simple procedural texture to the ground
    try
    {
      // Create a simple white texture programmatically
      Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().createManual(
          "GroundTexture", "General", Ogre::TEX_TYPE_2D, 256, 256, 0, Ogre::PF_R8G8B8A8);

      // Fill with a simple checker pattern
      Ogre::HardwarePixelBufferSharedPtr buffer = tex->getBuffer();
      buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
      const Ogre::PixelBox &pb = buffer->getCurrentLock();
      Ogre::uint8 *data = static_cast<Ogre::uint8 *>(pb.data);

      for (size_t y = 0; y < 256; ++y)
      {
        for (size_t x = 0; x < 256; ++x)
        {
          bool isWhite = ((x / 32) + (y / 32)) % 2 == 0;
          Ogre::uint8 color = isWhite ? 255 : 128;
          size_t index = (y * 256 + x) * 4;
          data[index + 0] = color; // R
          data[index + 1] = color; // G
          data[index + 2] = color; // B
          data[index + 3] = 255;   // A
        }
      }
      buffer->unlock();

      // Apply texture to ground material
      Ogre::TextureUnitState *texUnit = groundMat->getTechnique(0)->getPass(0)->createTextureUnitState();
      texUnit->setTexture(tex);
      texUnit->setTextureScale(4.0f, 4.0f); // Repeat texture 4x4 times
      std::cout << "Added checker texture to ground material" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Failed to create ground texture: " << e.what() << std::endl;
    }

    std::cout << "Created GroundLit material for RTSS (green with lighting)" << std::endl;
    std::cout << "RTSS initialized successfully for material compatibility" << std::endl;
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
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("materials", "FileSystem", "YARS");
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("YARS");
    std::cout << "Successfully loaded YARS materials directory" << std::endl;
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
      std::cout << "RENDERING FAILED (but window should still be open): " << e.what() << std::endl;
      std::cout << "Continuing simulation without 3D rendering..." << std::endl;
      errorShown = true;
    }
    // Continue without rendering - let the simulation run and window stay open
  }
  catch (const std::exception &e)
  {
    static bool errorShown = false;
    if (!errorShown)
    {
      std::cout << "RENDERING ERROR (but window should still be open): " << e.what() << std::endl;
      std::cout << "Continuing simulation without 3D rendering..." << std::endl;
      errorShown = true;
    }
    // Continue without rendering
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

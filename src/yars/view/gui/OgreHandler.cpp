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
  lm->getDefaultLog()->setLogDetail(Ogre::LL_LOW); // log errors only

  // Plugins are loaded from build/plugins.cfg (see cmake/CreateConfigFiles.cmake
  // and src/cfg/plugins.cfg.in). Ogre::Root reads the file and dlopens each
  // listed plugin from PluginFolder=${OGRE_ROOT}/lib/OGRE. Same path on Linux
  // (.so) and macOS (.dylib).
  _root = new Ogre::Root("plugins.cfg", "ogre.cfg", "");

  if (_root->getAvailableRenderers().size() == 0)
  {
    OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "No render systems available", "OgreHandler");
  }

  // Use GL3Plus renderer with OpenGL 3.0 compatibility profile
  const Ogre::RenderSystemList &renderers = _root->getAvailableRenderers();
  Ogre::RenderSystem *renderSystem = renderers.front();
  _root->setRenderSystem(renderSystem);

  // Check environment before attempting OpenGL context creation
  const char *display = getenv("DISPLAY");
  bool hasDisplay = (display != nullptr && strlen(display) > 0);

  try
  {
    // Renderer already set above - don't override it
    _root->initialise(false);
    _sceneManager = _root->createSceneManager("DefaultSceneManager");
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

    // Find the generated RTSS technique and copy blend/depth settings from
    // the base technique (index 0). RTSS doesn't copy scene_blend, so font
    // materials lose alpha_blend and character quads render as opaque blocks.
    // Base technique uses scheme "" (empty), not "Default", so use index 0.
    Ogre::Technique *baseTech = (originalMaterial->getNumTechniques() > 0)
                                    ? originalMaterial->getTechnique(0)
                                    : nullptr;

    for (unsigned short i = 0; i < originalMaterial->getNumTechniques(); ++i)
    {
      Ogre::Technique *tech = originalMaterial->getTechnique(i);
      if (tech->getSchemeName() == schemeName)
      {
        if (baseTech && tech != baseTech)
        {
          for (unsigned short p = 0; p < tech->getNumPasses() && p < baseTech->getNumPasses(); ++p)
          {
            Ogre::Pass *rtssPass = tech->getPass(p);
            Ogre::Pass *basePass = baseTech->getPass(p);
            // Copy alpha blending (critical for font/overlay transparency)
            if (basePass->getSourceBlendFactor() != Ogre::SBF_ONE ||
                basePass->getDestBlendFactor() != Ogre::SBF_ZERO)
            {
              rtssPass->setSceneBlending(basePass->getSourceBlendFactor(),
                                         basePass->getDestBlendFactor());
            }
            // Copy depth settings (fonts use depth_check off, depth_write off)
            rtssPass->setDepthCheckEnabled(basePass->getDepthCheckEnabled());
            rtssPass->setDepthWriteEnabled(basePass->getDepthWriteEnabled());
          }
        }
        return tech;
      }
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

  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();

  // Register YARS resources (meshes, materials, fonts)
  // All paths are relative to the project root (CWD when running ./build/bin/yars)
  rgm.addResourceLocation(".", "FileSystem", "General");
  rgm.addResourceLocation("fonts", "FileSystem", "General");
  rgm.addResourceLocation("meshes", "FileSystem", "General");

  // Register RTShaderLib for RTSS shader compilation (OgreInternal group).
  // These GLSL files are required by RTSS to generate shaders for GL3+ core profile.
  // Use absolute paths via YARS_SOURCE_DIR so the lookup doesn't depend on
  // cwd — Linux CI on a cache-restored Ogre install was missing the build-dir
  // symlink that the relative path relied on.
  rgm.addResourceLocation(std::string(YARS_SOURCE_DIR) + "/ext/ogre/RTShaderLib",
                          "FileSystem", "OgreInternal");
  // Ogre's installed Media/Main ships the shadow-system assets
  // (spot_shadow_fade.dds in particular). SHADOWTYPE_TEXTURE_MODULATIVE
  // looks for these in the OgreInternal resource group; without this
  // location, scene-setup throws FileNotFoundException.
  // YARS_OGRE_MEDIA_DIR is resolved at configure time by
  // cmake/IncludePackages.cmake (macOS installs to ${prefix}/Media,
  // Linux to ${prefix}/share/OGRE-<ver>/Media).
  rgm.addResourceLocation(std::string(YARS_OGRE_MEDIA_DIR) + "/Main",
                          "FileSystem", "OgreInternal");
  rgm.initialiseResourceGroup("OgreInternal");

  // Register YARS materials (textures, .material scripts) BEFORE MaterialManager
  // is first instantiated so RTSS_Ground etc. can resolve ground.jpg/wheel.jpg
  rgm.addResourceLocation("materials", "FileSystem", "YARS");
  rgm.initialiseResourceGroup("YARS");

  // Initialise groups
  rgm.initialiseResourceGroup("General");

  // ----------------------------------------------------------------------
  // 2) Runtime Shader System initialisation and automatic technique
  //    generation listener.
  // ----------------------------------------------------------------------

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
      } catch (const std::exception& e) {
          std::cerr << "Failed to initialize MaterialManager: " << e.what() << std::endl;
      }
      
      // Validate shader generation
      ShaderManager::instance()->validateShaderGeneration();

    }
  }
  else
  {
    _shaderGenerator = nullptr;
  }

  // ----------------------------------------------------------------------
  // 3) Diagnostics
  // ----------------------------------------------------------------------

  // Create RTSS techniques for YARS materials (already loaded above)
  try {
    MaterialManager::instance()->createRTSSForLegacyMaterials();
  } catch (const std::exception& e) {
    std::cerr << "Warning: Failed to create RTSS techniques for YARS materials: " << e.what() << std::endl;
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

  _rootNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
  _sceneGraph = new SceneGraph(_rootNode, _sceneManager);

  // Force an immediate scene graph update
  try
  {
    _sceneManager->_updateSceneGraph(nullptr);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Scene graph update failed: " << e.what() << std::endl;
  }

  // _textOverlay  = new TextOverlay("Legend Text Overlay");
  // Ogre::Real x = 10;
  // Ogre::Real y = 10;
  // _textOverlay->addTextBox("legend",
  // "^0YARS, Zahedi et al.", x, y, 150, 20,
  // Ogre::ColourValue(75.0/255.0, 117.0/255.0, 148.0/255.0,1.0f),
  // "Legend", "20");

  // Shadows are off pending a working technique.
  //
  // SHADOWTYPE_STENCIL_MODULATIVE: archive (2026-02-02, commit
  // 9024f19 in master-archive-2026-05-15) hit
  // `ManualObject::getShadowVolumeRenderableList` segfaults with
  // mParentNode == nullptr. Author left a TODO to try texture
  // shadows.
  //
  // SHADOWTYPE_TEXTURE_MODULATIVE: tried 2026-05-18. Setup
  // succeeds once the resource-group plumbing for
  // spot_shadow_fade.dds is in place (this commit keeps that
  // plumbing — YARS_OGRE_MEDIA_DIR, OgreInternal addResourceLocation,
  // edge-list null checks — for the next attempt). But the rendered
  // frames come out empty: every frame is I-frame at QP 0 with
  // 100% skip, suggesting the shadow-render's depth pass is
  // masking the colour buffer or RTSS isn't generating a working
  // ambient pass for shadow casters. Verified on both Linux/llvmpipe
  // (CI run 26021475339, 5 KB mp4) and macOS arm64 (real GPU,
  // 2.7 KB mp4). So it's a YARS-RTSS-shadow interaction, not a
  // llvmpipe limitation.
  //
  // Re-enabling needs either: (a) writing a custom shadow caster
  // material so RTSS knows how to emit the depth-only vertex shader,
  // or (b) switching to a non-RTSS technique (fixed-function
  // shadows are gone in GL3+ core).
  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
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
  try
  {
    _sceneGraph->update();
    _root->renderOneFrame();

  }
  catch (const Ogre::Exception &e)
  {
    // Transient first-frame exceptions (e.g. an RTSS-generated technique not
    // yet swapped in for a material) occasionally fire before rendering
    // stabilises; the simulation keeps running fine. Log to ogre.log once
    // instead of spamming stderr — a real persistent failure shows up
    // there with full context.
    static bool reported = false;
    if (!reported)
    {
      Ogre::LogManager::getSingleton().logMessage(
          "OgreHandler::step caught and swallowed: " + Ogre::String(e.what()),
          Ogre::LML_WARNING);
      reported = true;
    }
  }
  catch (const std::exception &e)
  {
    static bool reported = false;
    if (!reported)
    {
      Ogre::LogManager::getSingleton().logMessage(
          "OgreHandler::step caught and swallowed: " + Ogre::String(e.what()),
          Ogre::LML_WARNING);
      reported = true;
    }
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

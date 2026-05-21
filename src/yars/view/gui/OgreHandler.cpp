#include "OgreHandler.h"
#include "MaterialManager.h"
#include "PlanarShadowProjector.h"
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
#include <OGRE/RTShaderSystem/OgreShaderSubRenderState.h>
#include <OGRE/OgreShadowCameraSetupFocused.h>
#include <OGRE/OgreShadowCameraSetupPlaneOptimal.h>
#include <OGRE/OgreMovablePlane.h>


namespace yars {

// Fixed-light-direction shadow camera setup for YARS.
//
// The default Ogre shadow camera setups (Default, Focused) compute the
// shadow frustum based on the *eye camera's* position and direction, which
// makes shadows drift as the user moves the camera — clearly visible as
// shadows "sliding" across the floor when orbiting the scene.
//
// YarsFixedShadowCameraSetup ignores the eye camera entirely:
//  - Light direction is HARDCODED to (-1, -1, -1) (Ogre world space).
//  - Shadow camera is positioned at +50 units in the opposite light
//    direction, looking toward world origin.
//  - Orthographic frustum of 24×24 covers the typical YARS arena
//    (±4m walls) plus margin for dynamic objects.
//
// This produces stable, world-anchored shadows that don't move with the
// camera and reliably align with caster geometry.
class YarsFixedShadowCameraSetup : public Ogre::ShadowCameraSetup
{
public:
  void getShadowCamera(const Ogre::SceneManager *sm, const Ogre::Camera *cam,
                       const Ogre::Viewport *vp, const Ogre::Light *light,
                       Ogre::Camera *texCam, size_t iteration) const override
  {
    if (light->getType() != Ogre::Light::LT_DIRECTIONAL)
    {
      texCam->setCustomViewMatrix(false);
      texCam->setCustomProjectionMatrix(false);
      return;
    }

    // Hardcoded light direction. Light rays travel in this direction;
    // source is at -lightDir from origin.
    const Ogre::Vector3 lightDir =
        Ogre::Vector3(-1.0f, -1.0f, -1.0f).normalisedCopy();

    // Place the shadow camera at -lightDir * distance, looking toward origin.
    const Ogre::Real cameraDistance = 50.0f;
    const Ogre::Vector3 camPos = -lightDir * cameraDistance;

    // Build the view matrix by hand. Ogre uses a right-handed coordinate
    // system where the camera looks down its -Z axis.
    //
    // Camera basis in world space:
    //   forward (world dir camera looks)     = lightDir
    //   localZ (camera's local +Z)           = -forward = -lightDir
    //   localX (camera's local +X / right)   = up_world × localZ, normalised
    //   localY (camera's local +Y / up)      = localZ × localX
    Ogre::Vector3 worldUp = Ogre::Vector3::UNIT_Y;
    if (Ogre::Math::Abs(worldUp.dotProduct(lightDir)) >= 0.999f)
    {
      worldUp = Ogre::Vector3::UNIT_Z;
    }
    Ogre::Vector3 localZ = -lightDir;
    Ogre::Vector3 localX = worldUp.crossProduct(localZ);
    localX.normalise();
    Ogre::Vector3 localY = localZ.crossProduct(localX);
    localY.normalise();

    // View matrix: rows are camera basis vectors; translation = -R * camPos.
    Ogre::Affine3 view;
    view[0][0] = localX.x; view[0][1] = localX.y; view[0][2] = localX.z;
    view[1][0] = localY.x; view[1][1] = localY.y; view[1][2] = localY.z;
    view[2][0] = localZ.x; view[2][1] = localZ.y; view[2][2] = localZ.z;
    view[0][3] = -localX.dotProduct(camPos);
    view[1][3] = -localY.dotProduct(camPos);
    view[2][3] = -localZ.dotProduct(camPos);

    // Orthographic projection: 24×24 frustum, depth 1..200.
    const Ogre::Real halfWidth = 12.0f;
    const Ogre::Real halfHeight = 12.0f;
    const Ogre::Real n = 1.0f;
    const Ogre::Real f = 200.0f;
    Ogre::Matrix4 proj = Ogre::Matrix4::ZERO;
    proj[0][0] = 1.0f / halfWidth;
    proj[1][1] = 1.0f / halfHeight;
    proj[2][2] = -2.0f / (f - n);
    proj[2][3] = -(f + n) / (f - n);
    proj[3][3] = 1.0f;

    texCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    texCam->setNearClipDistance(n);
    texCam->setFarClipDistance(f);
    texCam->setCustomViewMatrix(true, view);
    texCam->setCustomProjectionMatrix(true, proj);
  }
};

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

      // Add SRS_SHADOW_MAPPING to the RTSS template render state BEFORE
      // any materials get their RTSS techniques generated below. The
      // template is applied to newly-generated techniques only, so it
      // must be set up before validateAllMaterials and
      // createRTSSForLegacyMaterials run. This wires every RTSS-managed
      // material as a shadow receiver under
      // SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED.
      //
      // Bootstrap LightCount to 1 on the RTSS render state.
      // IntegratedPSSM3::preAddToRenderState (the SRS_SHADOW_MAPPING
      // backend) bails out when getLightCount() == 0 and our directional
      // sun light is created further down in setupSceneManager, AFTER
      // material validation. setLightCountAutoUpdate stays true so adding
      // more lights still triggers regeneration; auto-update never
      // decreases the count, so seeding it with 1 is safe.
      {
        Ogre::RTShader::RenderState *schemRS =
            _shaderGenerator->getRenderState(Ogre::MSN_SHADERGEN);
        schemRS->setLightCount(1);
        // SRS_PER_PIXEL_LIGHTING is required for shadows to be visible:
        // SGX_ShadowPCF4 stores its result in lShadowFactor[i], but only
        // evaluateLight (per-pixel lighting) consumes that factor. Without
        // it, RTSS-generated shaders for textured materials end up doing
        // gl_FragColor = texel * baseColour with no lighting term — the
        // shadow factor is dead code and the floor stays at full
        // brightness regardless of the shadow texture content.
        schemRS->addTemplateSubRenderState(
            _shaderGenerator->createSubRenderState(Ogre::RTShader::SRS_PER_PIXEL_LIGHTING));
        schemRS->addTemplateSubRenderState(
            _shaderGenerator->createSubRenderState(Ogre::RTShader::SRS_SHADOW_MAPPING));
      }

      // Configure the SceneManager's shadow technique BEFORE materials get
      // their RTSS techniques. IntegratedPSSM3 (the SRS_SHADOW_MAPPING
      // backend) reads getShadowTextureCount() at technique-generation
      // time; if shadows are still SHADOWTYPE_NONE it produces a
      // degenerate shader and the receiver materials end up without a
      // vertex shader at all on GL3+ core.
      __setupShadows();

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
  // Shadow far distance on the LIGHT controls DefaultShadowCameraSetup's
  // ortho frustum size (shadowDist*2). Without this the light defaults to
  // 0 which Ogre approximates as eyeCam.nearClip*300 — varies wildly
  // between scenes and produces huge frustums. Pin to 15m for predictable
  // 30×30 coverage of the 8×8 YARS arena.
  lightSun->setShadowFarDistance(15.0f);
  node->setDirection(Ogre::Vector3(-1, -1, -1));
  lightSun->setDiffuseColour(1.2, 1.2, 1.0);  // Warm directional light
  lightSun->setSpecularColour(1.0, 1.0, 0.8); // Warm specular highlights
  node->attachObject(lightSun);

  // Planar projected shadows: render every shadow-casting entity a
  // second time, flattened onto the floor plane along the light
  // direction. Replaces the previous custom RTT pipeline.
  //
  // YARS uses Z-up world coordinates (see SceneGraphEnvironmentNode:
  // the ground plane is Plane(UNIT_Z, 0)). The PlanarShadowProjector
  // must use the matching floor normal and a Z-up light direction so
  // shadows project onto the z=0 plane.
  const Ogre::Plane floor(Ogre::Vector3::UNIT_Z, 0.0f);
  const Ogre::Vector3 lightDir(-1.0f, -1.0f, -1.0f);
  _planarShadows = std::make_unique<PlanarShadowProjector>(
      _sceneManager, floor, lightDir);

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

  // Note: __setupShadows() is invoked earlier — right after RTSS init,
  // before any materials get their RTSS techniques generated. Calling it
  // a second time here would re-set the technique unnecessarily.
}

void OgreHandler::__setupShadows()
{
  // SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED — RTSS-generated receivers.
  //
  // Previous attempts at SHADOWTYPE_TEXTURE_MODULATIVE with a custom
  // YARS/TextureShadowReceiver material + hand-written GLSL fell over
  // on a UV mismatch between caster and receiver passes that we
  // never tracked down (see docs/planning/shadows_state.md). The
  // integrated approach delegates per-material shadow lookups to
  // RTSS via the SRS_SHADOW_MAPPING sub-render-state, which uses the
  // canonical ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY auto-param in
  // a generated shader that lives inside each material's own pass.
  // No separate modulating pass; no UV math we have to maintain.
  //
  // Custom YARS/TextureShadowCaster is still required: the default
  // Ogre/TextureShadowCaster is fixed-function and fails to link on
  // GL3+ core ("technique has no Vertex Shader").
  //
  // YarsFixedShadowCameraSetup pins the shadow camera to the world
  // origin (looking down -lightDir from +50 units) so shadows are
  // anchored to geometry, not to the eye camera.
  try
  {
    // SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED — RTSS-generated
    // receivers via SRS_SHADOW_MAPPING (wired in setupSceneManager).
    //
    // We tried ADDITIVE_INTEGRATED + PF_DEPTH16 (the depth-shadow path
    // used by Ogre's Character/Terrain samples) — every receiver
    // material failed to compile because Ogre's GLSL preprocessor
    // doesn't expand the SAMPLER2DSHADOW macro on GL3+ core. Same
    // class of blocker as the OGRE_UNIFORMS / MAIN_PARAMETERS issue
    // that stops stencil shadows from working here. Reverted.
    //
    // Result of the colour-shadow path: caster pass writes the
    // shadow texture correctly (verified by dumping it to PNG), and
    // RTSS-generated receiver shaders DO compute lShadowFactor and
    // feed it into evaluateLight — BUT lShadowFactor is uniformly
    // ~1.0 across the visible ground regardless of caster geometry.
    // Visualizing the receiver UV directly (hand-edited shader)
    // showed smooth UVs in [0,1] that simply don't land where the
    // caster wrote silhouettes. This is the same Ogre-14
    // texture_worldviewproj_matrix bug documented in
    // docs/planning/shadows_attempts_log.md, now also reproduced with
    // RTSS-generated receiver code.
    //
    // Until that auto-param bug is fixed (or worked around), shadows
    // are visually invisible. Disabling the technique outright so the
    // scene renders with no shadow overhead while a real fix is
    // researched. Re-enable by switching back to
    // SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED and uncommenting the
    // setup block below — the RTSS template wiring earlier in
    // setupSceneManager is already in place.
    _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    // _sceneManager->setShadowTextureSize(2048);
    // _sceneManager->setShadowTextureCount(1);
    // _sceneManager->setShadowFarDistance(15.0f);
    // _sceneManager->setShadowColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
    // _sceneManager->setShadowCasterRenderBackFaces(false);
    // _sceneManager->setShadowCameraSetup(
    //     Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));
    // auto casterMat = Ogre::MaterialManager::getSingleton().getByName("YARS/TextureShadowCaster");
    // if (casterMat)
    // {
    //   casterMat->load();
    //   _sceneManager->setShadowTextureCasterMaterial(casterMat);
    // }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Shadow setup failed: " << e.what() << std::endl;
    _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  }
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
    if (_planarShadows) _planarShadows->update();
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

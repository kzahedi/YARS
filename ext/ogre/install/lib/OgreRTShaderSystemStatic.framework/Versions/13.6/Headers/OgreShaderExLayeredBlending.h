/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _OgreShaderExLayeredBlending_
#define _OgreShaderExLayeredBlending_

#include "OgreShaderPrerequisites.h"
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderFFPTexturing.h"
#include "OgreShaderSubRenderState.h"

namespace Ogre {
namespace RTShader {

class _OgreRTSSExport LayeredBlending : public FFPTexturing
{
public:
    enum BlendMode
    {
        LB_Invalid = -1,
        LB_FFPBlend,
        LB_BlendNormal,
        LB_BlendLighten,            
        LB_BlendDarken,     
        LB_BlendMultiply,
        LB_BlendAverage,    
        LB_BlendAdd,
        LB_BlendSubtract,
        LB_BlendDifference,
        LB_BlendNegation,
        LB_BlendExclusion,
        LB_BlendScreen,
        LB_BlendOverlay,
        LB_BlendSoftLight,
        LB_BlendHardLight,
        LB_BlendColorDodge,
        LB_BlendColorBurn, 
        LB_BlendLinearDodge,
        LB_BlendLinearBurn,
        LB_BlendLinearLight,
        LB_BlendVividLight,
        LB_BlendPinLight,
        LB_BlendHardMix,
        LB_BlendReflect,
        LB_BlendGlow,
        LB_BlendPhoenix,
        LB_BlendSaturation,
        LB_BlendColor,
        LB_BlendLuminosity,
        LB_MaxBlendModes
    };

    enum SourceModifier
    {
        SM_Invalid = -1,
        SM_None,
        SM_Source1Modulate,
        SM_Source2Modulate,
        SM_Source1InvModulate,
        SM_Source2InvModulate,
        SM_MaxSourceModifiers
    };

    struct TextureBlend
    {
        TextureBlend() : blendMode(LB_Invalid), sourceModifier(SM_Invalid), customNum(0) {}

        //The blend mode to use
        BlendMode blendMode;
        //The source modification to use
        SourceModifier sourceModifier;
        // The number of the custom param controlling the source modification
        int customNum;
        //The parameter controlling the source modification
        ParameterPtr modControlParam;
    };


    /** Class default constructor */
    LayeredBlending();

    /** 
    @see SubRenderState::getType.
    */
    const Ogre::String& getType                 () const override;


    /** 
    Set the blend mode of the given texture unit layer with the previous layer.
    @param index The texture unit texture. Textures units (index-1) and (index) will be blended.
    @param mode The blend mode to apply.
    */
    void setBlendMode(unsigned short index, BlendMode mode);

    /// @overload
    bool setBlendMode(uint16 index, const String& mode);

    /** 
    Return the blend mode of the given texture unit index.
    */
    BlendMode getBlendMode(unsigned short index) const;

    

    /** 
    Set the source modifier parameters for a given texture unit
    @param index Texture blend index
    @param modType The source modification type to use
    @param customNum The custom parameter number used to control the modification
    */
    void setSourceModifier(unsigned short index, SourceModifier modType, int customNum);

    /// @overload
    bool setSourceModifier(unsigned short index, const String& modType, int customNum);

    /** 
    Returns the source modifier parameters for a given texture unit
    @return True if a valid modifier exist for the given texture unit
    @param index Texture blend index
    @param modType The source modification type to use
    @param customNum The custom parameter number used to control the modification
    */
    bool getSourceModifier(unsigned short index, SourceModifier& modType, int& customNum) const;

    /** 
    @see SubRenderState::copyFrom.
    */
    void copyFrom(const SubRenderState& rhs) override;

    static String Type;

// Protected methods
protected:
    
    /** 
    @see SubRenderState::resolveParameters.
    */
    bool resolveParameters(ProgramSet* programSet) override;

    /** 
    @see SubRenderState::resolveDependencies.
    */
    bool resolveDependencies(Ogre::RTShader::ProgramSet* programSet) override;


    void addPSBlendInvocations(Function* psMain,
                                       ParameterPtr arg1,
                                       ParameterPtr arg2,
                                       ParameterPtr texel,
                                       int samplerIndex,
                                       const LayerBlendModeEx& blendMode,
                                       const int groupOrder, 
                                       Operand::OpMask targetChannels) override;
    /** 
    Adds the function invocation to the pixel shader which will modify
    the blend sources according to the source modification parameters.
    */
    void addPSModifierInvocation(Function* psMain, 
                                 int samplerIndex, 
                                 ParameterPtr arg1,
                                 ParameterPtr arg2,
                                 const int groupOrder, 
                                 Operand::OpMask targetChannels);

    // Attributes.
protected:
    std::vector<TextureBlend> mTextureBlends;

};



/** 
A factory that enables creation of LayeredBlending instances.
@remarks Sub class of SubRenderStateFactory
*/
class _OgreRTSSExport LayeredBlendingFactory : public SubRenderStateFactory
{
public:

    /** 
    @see SubRenderStateFactory::getType.
    */
    const String& getType() const override;

    /** 
    @see SubRenderStateFactory::createInstance.
    */
    SubRenderState* createInstance(ScriptCompiler* compiler, PropertyAbstractNode* prop, TextureUnitState* texState, SGScriptTranslator* translator) override;

    /** 
    @see SubRenderStateFactory::writeInstance.
    */
    void writeInstance(MaterialSerializer* ser, SubRenderState* subRenderState, const TextureUnitState* srcTextureUnit, const TextureUnitState* dstTextureUnit) override;

    
protected:

    /** 
    @see SubRenderStateFactory::createInstanceImpl.
    */
    SubRenderState* createInstanceImpl() override;

    /** 
    Returns the LayeredBlending sub-rener state previously created for this material/pass.
    if no such sub-render state exists creates a new one
    @param translator compiler
    */
    LayeredBlending* createOrRetrieveSubRenderState(SGScriptTranslator* translator);
};

} // namespace RTShader
} // namespace Ogre

#endif // RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#endif // _OgreShaderExLayeredBlending_

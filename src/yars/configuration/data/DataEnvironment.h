#ifndef __DATA_ENVIRONMENT_H__
#define __DATA_ENVIRONMENT_H__

//#include "types/LightSource.h"

#include "DataNode.h"
#include "DataObjects.h"
#include "DataPointLightSource.h"
#include "DataAmbientLight.h"
#include "DataObjectFactory.h" // must be here, otherwise DataBox and the others are not accessible
// #include "DataTexture.h"

#include "DataMacroInstance.h"
#include "MacroUserInterface.h"

# define ENVIRONMENT                        "environment"
# define YARS_STRING_ENVIRONMENT            (char*)ENVIRONMENT
# define YARS_STRING_ENVIRONMENT_DEFINITION (char*)ENVIRONMENT DIVIDER DEFINITION

#include <string>
#include <vector>

using namespace std;

class DataEnvironment : public DataNode, public MacroUserInterface
{
  public:
    /**
     * @brief Constrcutor
     *
     * @param parent
     */
    DataEnvironment(DataNode *parent);

    /**
     * @brief Destructor
     *
     * @param parent
     */
    virtual ~DataEnvironment();

    /**
     * @brief Returns the name of the environment.
     *
     * @return name
     */
    string name();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns pointer to begin of the vector of DataObject.
     *
     * @return iterator
     * @sa DataObject
     * @sa vector
     */
    DataObjects::iterator o_begin();

    /**
     * @brief Returns the pointer to the end of the vector of DataObject.
     *
     * @return iterator
     * @sa DataObject
     * @sa vector
     */
    DataObjects::iterator o_end();

    /**
     * @brief Number of objects in the environment.
     *
     * @return size
     */
    int o_size();

    /**
     * @brief Returns pointer to the first geom of the vector of DataObject.
     *
     * @return iterator
     * @sa DataObject
     * @sa vector
     */
    DataObjects::iterator g_begin();

    /**
     * @brief Returns the pointer to the last geom of the vector of DataObject.
     *
     * @return iterator
     * @sa DataObject
     * @sa vector
     */
    DataObjects::iterator g_end();

    /**
     * @brief Number of geoms in the environment.
     *
     * @return size
     */
    int g_size();

    /**
     * @brief Returns the n-th environment.
     *
     * @param index
     *
     * @return DataObject
     * @sa DataObject
     */
    DataObject* object(int index);

    /**
     * @brief Returns the iterator to the beginning of list of DataPointLightSource.
     *
     * @return iterator
     * @sa DataPointLightSource
     * @sa vector
     */
    std::vector<DataPointLightSource*>::iterator l_begin();

    /**
     * @brief Returns the iterator to the end of list of DataPointLightSource.
     *
     * @return iterator
     * @sa DataPointLightSource
     * @sa vector
     */
    std::vector<DataPointLightSource*>::iterator l_end();

    /**
     * @brief Returns the number of light sources.
     *
     * @return number of light sources (int)
     */
    int l_size();

    /**
     * @brief Returns the n-th light source.
     *
     * @param index
     *
     * @return light source
     * @sa DataPointLightSource
     */
    DataPointLightSource* lightSource(int index);

    DataAmbientLight* ambientLight();

    void setMacros(DataMacros *macros);
    DataMacros* macros();
    // DataTexture* texture();
    bool groundIsFixed();

    bool useFog();
    double fogIntensity();

    P3D normal();
    bool groundGiven();

    DataEnvironment* copy();

    string texture();
    vector<DataMeshVisualisation*>::iterator m_begin();
    vector<DataMeshVisualisation*>::iterator m_end();
    int                                      m_size();

    static void createXsd(XsdSpecification *spec);

    void resetTo(DataEnvironment *other);
    double gravitation(int index);

  protected:
    void                       __gatherGeoms();

  private:
    void __applyMacros();

    string                           _name;
    DataObjects                      _objects;
    DataObjects                      _geoms;
    std::vector<DataPointLightSource*>    _lightSources;
    std::vector<DataMacroInstance*>  _macros;
    DataMacros                      *_macrosDefinitions;
    P3D                              _normal;
    bool                             _fixedGround;
    bool                             _useFog;
    double                            _fogIntensity;
    string                           _texture;
    vector<DataMeshVisualisation*>   _meshes;
    bool                             _groundGiven;
    double                           *_gravitation;
    DataAmbientLight*                _ambientLight;
};

#endif // __DATA_ENVIRONMENT_H__

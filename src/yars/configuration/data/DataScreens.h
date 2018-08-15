#ifndef __DATA_SCREENS_H__
#define __DATA_SCREENS_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataScreen.h>
#include <yars/configuration/data/DataFollowables.h>
#include <yars/configuration/data/DataCameraConfig.h>
#include <yars/configuration/data/DataRecording.h>

#include <yars/types/RHDimension.h>

#include <vector>
#include <string>

# define YARS_STRING_SCREEN_CONFIG            (char*)"screens"
# define YARS_STRING_SCREEN_CONFIG_DEFINITION (char*)"screens_definition"

using namespace std;

/**
 * @brief Contains all screen configurations.
 */
class DataScreens : public std::vector<DataScreen*>, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataScreens(DataNode *parent);

    /**
     * @brief Destructor. Deletes all screen configurations.
     */
    virtual ~DataScreens();

    /**
     * @brief Add a screen configuration
     *
     * @param screen
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the n-th screen definition
     *
     * @param index
     *
     * @return screen difinition
     * @sa DataScreen
     */
    DataScreen* screen(int index);

    /**
     * @brief Returns the screen definition given by name.
     *
     * Returns NULL if no screen definition with the given name is found.
     *
     * @param name
     *
     * @return screen definition
     * @sa DataScreen
     */
    DataScreen* screen(string name);

    DataFollowables* followables();

    DataRecording* recording();

    DataScreens* copy();

    DataCameraConfig* cameraConfig();

    string sky();
    bool hasVisible();

    bool visualiseJoints();
    RHDimension jointsVisualisationDimension();
    string xAxisTexture();
    string yAxisTexture();
    string zAxisTexture();

    static void createXsd(XsdSpecification *spec);

  private:
    DataScreen       *_default;
    DataFollowables  *_followables;
    string            _sky;
    DataCameraConfig *_cameraConfig;
    DataRecording    *_recording;
    bool              _visualiseJoints;
    RHDimension       _jointsAxisDimension;
    string            _xTexture;
    string            _yTexture;
    string            _zTexture;
};
#endif // __DATA_SCREENS_H__

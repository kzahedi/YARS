#ifndef __DATA_CAMERA_H__
#define __DATA_CAMERA_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/types/P3D.h>

# define YARS_STRING_CAMERA                   (char*)"camera"
# define YARS_STRING_SCREEN_CAMERA_DEFINITION (char*)"camera_screen_definition"

#include <string>

using namespace std;

/**
 * @brief Camera Definition
 */
class DataCamera : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataCamera(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataCamera();

    P3D position();
    P3D lookAt();
    P3D up();

    void setPosition(P3D p);
    void setLookAt(P3D p);

    void add(DataParseElement *element);
    DataCamera* copy();

    static void createXsd(XsdSpecification *spec);

  private:
    P3D _position;
    P3D _lookAt;
    P3D _up;
};

#endif // __DATA_CAMERA_H__


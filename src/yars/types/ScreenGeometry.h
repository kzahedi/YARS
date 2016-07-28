#ifndef __SCREEN_GEOMETRY_H__
#define __SCREEN_GEOMETRY_H__

#include <ostream>

# define SG_CENTER -1000
# define SG_END    -1

#  ifdef USE_THREAD_SAFE
#    include "pthread.h"
#  endif // USE_THREAD_SAFE

/** \brief This class implements a container to the window dimensions.
 *
 * The window dimensions are defined by width and height. The purpose of this
 * class is to implement a thread safe container to this values.
 *
 * \todo Unit test
 */
class ScreenGeometry
{
  public:
    /** \brief Standard constructor
     *
     * Standard values are set to 640x480
     */
    ScreenGeometry();
#ifndef _MSC_VER
    ScreenGeometry(ScreenGeometry& sg);
#endif // _MSC_VER
    ScreenGeometry(const ScreenGeometry& sg);

    /** \brief Returns the width.
     *
     * This function is thread safe, i.e. it blocks if another thread accesses
     * the same variable.
     *
     * \param[out] unsigned int width
     */
    unsigned int width();
    /** \brief Sets the width.
     *
     * This function is thread safe, i.e. it blocks if another thread accesses
     * the same variable
     *
     * \param[in] unsigned int width
     */
    void setWidth(unsigned int width);

    /** \brief Returns the height.
     *
     * This function is thread safe, i.e. it blocks if another thread accesses
     * the same variable.
     *
     * \param[out] unsigned int height
     */
    unsigned int height();
    /** \brief Sets the height.
     *
     * This function is thread safe, i.e. it blocks if another thread accesses
     * the same variable.
     *
     * \param[in] unsigned int height
     */
    void setHeight(unsigned int height);

    /** \brief Sets the dimensions.
     *
     * This function is thread safe, i.e. it blocks if another thread accesses
     * the same variables.
     *
     * \param[in] unsigned int width
     * \param[in] unsigned int height
     */
    void setDimensions(unsigned int width, unsigned int height);
    /** \brief Returns the dimensions.
     *
     * This function is thread safe, i.e. it blocks if another thread accesses
     * the same variables.
     *
     * \param[out] unsigned int width
     * \param[out] unsigned int height
     */
    void getDimensions(unsigned int *width, unsigned int *height);

    ScreenGeometry& operator=(const ScreenGeometry &d);

    int x();
    int y();
    void setX(int x);
    void setY(int y);
    void setXY(int x, int y);

    friend std::ostream& operator<<(std::ostream& str, const ScreenGeometry& s)
    {
      str << s._x << ", " << s._y << ": " << s._width << "x" << s._height;
      return str;
    };

  private:
    unsigned int _width;
    unsigned int _height;
    int _x;
    int _y;

#ifdef USE_THREAD_SAFE
    pthread_mutex_t _dimensionMutex;
#endif // USE_THREAD_SAFE
};

#endif // __SCREEN_GEOMETRY_H__

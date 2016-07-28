#ifndef __WIDHT_HEIGHT_DEPTH_DIMENSION_H__
#define __WIDHT_HEIGHT_DEPTH_DIMENSION_H__

#include <yars/defines/types.h>

#include <ostream>

/** \brief This class implements a three dimensional point.
 *
 * Point values are accessed directly by width, height, depth:
 *
 *
 \verbatim
 P3D p;
 p.width;
 p.height;
 p.depth; \endverbatim
 *
 * P3D is pre-defined to __WHDDimension<yReal>
 *
 * \todo Unit test
 */
template<class T>
class __WHDDimension
{
  public:
    /** \brief width-value of the three dimension point.
     */
    T width;
    /** \brief height-value of the three dimension point.
     */
    T height;
    /** \brief depth-value of the three dimension point.
     */
    T depth;
    /** \brief Standard constructor.
     *
     * Sets width, height, depth to 0.0
     */
    __WHDDimension<T>()
    {
      width = 0;
      height = 0;
      depth = 0;
    };

    /** \brief Copy constructor with pointer to array.
     *
     * Takes an array of length 3 and sets the point values accordingly:
     \verbatim
     width = p[0];
     height = p[1];
     depth = p[2]; \endverbatim
     *
     * Note that this function is not fault tolerant, as the length of the array
     * cannot be checked.
     * \param[in] T* pointer, pointer to the array of length 3
     */
    __WHDDimension<T>(const T* p)
    {
      width = p[0];
      height = p[1];
      depth = p[2];
    };

    /** \brief Copy constructor with the values.
     *
     * Takes three values and sets the point accordingly.
      \verbatim
     width = _width;
     height = _height;
     depth = _depth; \endverbatim
     *
     * \param[in] T _width, width-value
     * \param[in] T _height, height-value
     * \param[in] T _depth, depth-value
     */
    __WHDDimension<T>(const T _width, const T _height, const T _depth)
    {
      width = _width;
      height = _height;
      depth = _depth;
    };


    /** \brief Copy constructor.
     *
     * Takes a __WHDDimension<T> and copies its values.
      \verbatim
     width = p.width;
     height = p.height;
     depth = p.depth; \endverbatim
     *
     * \param[in] __WHDDimension<T> *p
     */
    __WHDDimension<T>(const __WHDDimension<T> *p)
    {
      width = p->width;
      height = p->height;
      depth = p->depth;
    };

    /** \brief Copy operator.
     *
     * Copies the values of point b to a:
      \verbatim
     a = b; \endverbatim
     *
     * \param[in] __WHDDimension<T> &b
     */
    __WHDDimension<T> & operator=(const __WHDDimension<T> &b)
    {
      width = b.width;
      height = b.height;
      depth = b.depth;
      return *this;
    };

    /** \brief Copy operator with single value.
     *
     * Copies the values of scala b to a:
     \verbatim
     a = value; \endverbatim
     * which is equivalent to
      \verbatim
     a.width = a.height = a.depth = value; \endverbatim
     *
     * \param[in] T value
     */
    __WHDDimension<T> & operator=(const T value)
    {
      width = value;
      height = value;
      depth = value;
      return *this;
    };

    /** \brief Copy operator with pointer to an array of length three.
     *
     * Copies the values of the array b of length 3 to a:
      \verbatim
     a = b; \endverbatim
     * which is equivalent to
      \verbatim
     a.width = b[0];
     a.height = b[1];
     a.depth = b[2]; \endverbatim
     *
     * \param[in] __WHDDimension<T> &b
     */
    __WHDDimension<T> & operator=(const T* values)
    {
      width = values[0];
      height = values[1];
      depth = values[2];
      return *this;
    };

    /** \brief Multiplication operator.
     *
     * Multiplies the values of the point with the given value.
     *
     \verbatim
     a *= b; \endverbatim
     * which is equivalent to
     *
     \verbatim
     a.width *= b;
     a.height *= b;
     a.depth *= b; \endverbatim
     *
     * \param[in] T b
     */
    __WHDDimension<T> & operator*=(const T value)
    {
      width *= value;
      height *= value;
      depth *= value;
      return *this;
    };

    /** \brief Division operator.
     *
     * Divided the values of the point with the given value.
     *
     \verbatim
     a /= b; \endverbatim
     * which is equivalent to
     *
     \verbatim
     a.width /= b;
     a.height /= b;
     a.depth /= b; \endverbatim
     *
     * \param[in] T b
     */
    __WHDDimension<T> & operator/=(const T value)
    {
      width /= value;
      height /= value;
      depth /= value;
      return *this;
    };

    /** \brief Addition operator.
     *
     * Adds two points.
     *
     \verbatim
     a += b; \endverbatim
     * which is equivalent to
     *
     \verbatim
     a.width += b.width;
     a.height += b.height;
     a.depth += b.depth; \endverbatim
     *
     * \param[in] __WHDDimension<T> b
     */
    __WHDDimension<T> & operator+=(const __WHDDimension<T> &a)
    {
      width += a.width;
      height += a.height;
      depth += a.depth;
      return *this;
    };

    /** \brief Subtraction operator.
     *
     * Subtracts two points.
     *
     \verbatim
     a -= b; \endverbatim
     * which is equivalent to
     *
     \verbatim
     a.width -= b.width;
     a.height -= b.height;
     a.depth -= b.depth; \endverbatim
     *
     * \param[in] __WHDDimension<T> b
     */
    __WHDDimension<T> & operator-=(const __WHDDimension<T> &b)
    {
      width = width - b.width;
      height = height - b.height;
      depth = depth - b.depth;
      return *this;
    };

    /** \brief Addition operator.
     *
     * Returns the value of the addition of two points.
     *
     \verbatim
     c = a + b; \endverbatim
     * which is equivalent to
     *
     \verbatim
     c.width = a.width + b.width;
     c.height = a.height + b.height;
     c.depth = a.depth + b.depth; \endverbatim
     *
     * \param[in] __WHDDimension<T> b
     */
    const __WHDDimension<T> operator+(const __WHDDimension<T> &b) const
    {
      __WHDDimension<T> r = *this;
      r += b;
      return r;
    };

    /** \brief Subtracts operator.
     *
     * Returns the value of the subtraction of two points.
     *
     \verbatim
     c = a - b; \endverbatim
     * which is equivalent to
     *
     \verbatim
     c.width = a.width - b.width;
     c.height = a.height - b.height;
     c.depth = a.depth - b.depth; \endverbatim
     *
     * \param[in] __WHDDimension<T> b
     */
    const __WHDDimension<T> operator-(const __WHDDimension<T> &b) const
    {
      __WHDDimension<T> r = *this;
      r -= b;
      return r;
    };

    /** \brief Scaling operator.
     *
     * Returns the points scaled by a scalar.
     *
     \verbatim
     c = a * value; \endverbatim
     * which is equivalent to
     *
     \verbatim
     c.width = a.width * b;
     c.height = a.height * b;
     c.depth = a.depth * b; \endverbatim
     *
     * \param[in] T value
     */
    const __WHDDimension<T> operator*(const T value) const
    {
      __WHDDimension<T> r = *this;
      r *= value;
      return r;
    };

    void normalise()
    {
      yReal l = sqrt(width * width + height * height + depth * depth);
      width /= l;
      height /= l;
      depth /= l;
    }

    yReal length()
    {
      return sqrt(width * width + height * height + depth * depth);
    }

    bool operator==(const __WHDDimension<T> a)
    {
      return (width == a.width) && (height == a.height) && (depth == a.depth);
    }

    /**
     * @brief Return the volume.
     *
     * @return volume
     */
    double volume()
    {
      return width * height * depth;
    }

    /** \brief Debug function.
     *
     * Gives the values as an input to an ostream:
     *
     \verbatim
     __WHDDimension<yReal> p;
     cout << p << endl; \endverbatim
     * which gives
     \verbatim
     width, height, depth \endverbatim
     * \param[in] std::ostream stream
     * \param[in] __WHDDimension<T> p
     *
     */
    friend std::ostream& operator<<(std::ostream& str, const __WHDDimension<T>& p)
    {
      str << p.width << ", " << p.height << ", " << p.depth;
      return str;
    };

};

/** Pre-defined standard __WHDDimension class with yReal values */
typedef __WHDDimension<yReal> WHDDimension;

#endif // __WIDHT_HEIGHT_DEPTH_DIMENSION_H__


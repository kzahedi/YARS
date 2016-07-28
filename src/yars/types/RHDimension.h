#ifndef __RADIUS_HEIGHT_DIMENSION_H__
#define __RADIUS_HEIGHT_DIMENSION_H__

#include <yars/defines/types.h>

#include <ostream>
#include <math.h>

/** \brief This class implements a three dimensional point.
 *
 * Point values are accessed directly by height, radius:
 *
 *
 \verbatim
 P3D p;
 p.height;
 p.radius; \endverbatim
 *
 * P3D is pre-defined to __RHDimension<yReal>
 *
 * \todo Unit test
 */
template<class T>
class __RHDimension
{
  public:
    /** \brief height-value of the three dimension point.
     */
    T height;
    /** \brief radius-value of the three dimension point.
     */
    T radius;
    /** \brief Standard constructor.
     *
     * Sets  height, radius to 0.0
     */
    __RHDimension<T>()
    {
      height = 0;
      radius = 0;
    };

    /** \brief Copy constructor with pointer to array.
     *
     * Takes an array of length 3 and sets the point values accordingly:
     \verbatim
     height = p[1];
     radius = p[2]; \endverbatim
     *
     * Note that this function is not fault tolerant, as the length of the array
     * cannot be checked.
     * \param[in] T* pointer, pointer to the array of length 3
     */
    __RHDimension<T>(const T* p)
    {
      height = p[1];
      radius = p[2];
    };

    /** \brief Copy constructor with the values.
     *
     * Takes three values and sets the point accordingly.
      \verbatim
     height = _height;
     radius = _radius; \endverbatim
     *
     * \param[in] T _height, height-value
     * \param[in] T _radius, radius-value
     */
    __RHDimension<T>( const T _height, const T _radius)
    {
      height = _height;
      radius = _radius;
    };


    /** \brief Copy constructor.
     *
     * Takes a __RHDimension<T> and copies its values.
      \verbatim
     height = p.height;
     radius = p.radius; \endverbatim
     *
     * \param[in] __RHDimension<T> *p
     */
    __RHDimension<T>(const __RHDimension<T> *p)
    {
      height = p->height;
      radius = p->radius;
    };

    /** \brief Copy operator.
     *
     * Copies the values of point b to a:
      \verbatim
     a = b; \endverbatim
     *
     * \param[in] __RHDimension<T> &b
     */
    __RHDimension<T> & operator=(const __RHDimension<T> &b)
    {
      height = b.height;
      radius = b.radius;
      return *this;
    };

    /** \brief Copy operator with single value.
     *
     * Copies the values of scala b to a:
     \verbatim
     a = value; \endverbatim
     * which is equivalent to
      \verbatim
      a.height = a.radius = value; \endverbatim
     *
     * \param[in] T value
     */
    __RHDimension<T> & operator=(const T value)
    {
      height = value;
      radius = value;
      return *this;
    };

    /** \brief Copy operator with pointer to an array of length three.
     *
     * Copies the values of the array b of length 3 to a:
      \verbatim
     a = b; \endverbatim
     * which is equivalent to
      \verbatim
     a.height = b[1];
     a.radius = b[2]; \endverbatim
     *
     * \param[in] __RHDimension<T> &b
     */
    __RHDimension<T> & operator=(const T* values)
    {
      height = values[1];
      radius = values[2];
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
     a.height *= b;
     a.radius *= b; \endverbatim
     *
     * \param[in] T b
     */
    __RHDimension<T> & operator*=(const T value)
    {
      height *= value;
      radius *= value;
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
     a.height /= b;
     a.radius /= b; \endverbatim
     *
     * \param[in] T b
     */
    __RHDimension<T> & operator/=(const T value)
    {
      height /= value;
      radius /= value;
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
     a.height += b.height;
     a.radius += b.radius; \endverbatim
     *
     * \param[in] __RHDimension<T> b
     */
    __RHDimension<T> & operator+=(const __RHDimension<T> &a)
    {
      height += a.height;
      radius += a.radius;
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
     a.height -= b.height;
     a.radius -= b.radius; \endverbatim
     *
     * \param[in] __RHDimension<T> b
     */
    __RHDimension<T> & operator-=(const __RHDimension<T> &b)
    {
      height = height - b.height;
      radius = radius - b.radius;
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
     c.height = a.height + b.height;
     c.radius = a.radius + b.radius; \endverbatim
     *
     * \param[in] __RHDimension<T> b
     */
    const __RHDimension<T> operator+(const __RHDimension<T> &b) const
    {
      __RHDimension<T> r = *this;
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
     c.height = a.height - b.height;
     c.radius = a.radius - b.radius; \endverbatim
     *
     * \param[in] __RHDimension<T> b
     */
    const __RHDimension<T> operator-(const __RHDimension<T> &b) const
    {
      __RHDimension<T> r = *this;
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
     c.height = a.height * b;
     c.radius = a.radius * b; \endverbatim
     *
     * \param[in] T value
     */
    const __RHDimension<T> operator*(const T value) const
    {
      __RHDimension<T> r = *this;
      r *= value;
      return r;
    };

    void normalise()
    {
      yReal l = sqrt(height * height + radius * radius);
      height /= l;
      radius /= l;
    }

    yReal length()
    {
      return sqrt(height * height + radius * radius);
    }

    bool operator==(const __RHDimension<T> a)
    {
      return (height == a.height) && (radius == a.radius);
    }

    /** \brief Debug function.
     *
     * Gives the values as an input to an ostream:
     *
     \verbatim
     __RHDimension<yReal> p;
     cout << p << endl; \endverbatim
     * which gives
     \verbatim
     height, radius \endverbatim
     * \param[in] std::ostream stream
     * \param[in] __RHDimension<T> p
     *
     */
    friend std::ostream& operator<<(std::ostream& str, const __RHDimension<T>& p)
    {
      str << p.height << ", " << p.radius;
      return str;
    };

    double volume()
    {
      return radius * radius * height * M_PI;
    }

};

/** Pre-defined standard __RHDimension class with yReal values */
typedef __RHDimension<yReal> RHDimension;

#endif // __RADIUS_HEIGHT_DIMENSION_H__



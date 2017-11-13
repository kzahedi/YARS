#ifndef __POINT_2D_H__
#define __POINT_2D_H__

#include <yars/defines/types.h>

#include <ostream>

/** \brief This class implements a three dimensional point.
 *
 * Point values are accessed directly by x, y:
 *
 *
 \verbatim
 P2D p;
 p.x;
 p.y; \endverbatim
 *
 * P2D is pre-defined to __Point2D<double>
 *
 * \todo Unit test
 */
template<class T>
class __Point2D
{
  public:
    /** \brief x-value of the three dimension point.
     */
    T x;
    /** \brief y-value of the three dimension point.
     */
    T y;
    /** \brief Standard constructor.
     *
     * Sets x, y to 0.0
     */
    __Point2D<T>()
    {
      x = 0;
      y = 0;
    };

    /** \brief Copy constructor with pointer to array.
     *
     * Takes an array of length 3 and sets the point values accordingly:
     \verbatim
     x = p[0];
     y = p[1]; \endverbatim
     *
     * Note that this function is not fault tolerant, as the length of the array
     * cannot be checked.
     * \param[in] T* pointer, pointer to the array of length 3
     */
    __Point2D<T>(const T* p)
    {
      x = p[0];
      y = p[1];
    };

    /** \brief Copy constructor with the values.
     *
     * Takes three values and sets the point accordingly.
      \verbatim
     x = _x;
     y = _y; \endverbatim
     *
     * \param[in] T _x, x-value
     * \param[in] T _y, y-value
     */
    __Point2D<T>(const T _x, const T _y)
    {
      x = _x;
      y = _y;
    };


    /** \brief Copy constructor.
     *
     * Takes a __Point2D<T> and copies its values.
      \verbatim
     x = p.x;
     y = p.y; \endverbatim
     *
     * \param[in] __Point2D<T> *p
     */
    __Point2D<T>(const __Point2D<T> *p)
    {
      x = p->x;
      y = p->y;
    };

    /** \brief Copy operator.
     *
     * Copies the values of point b to a:
      \verbatim
     a = b; \endverbatim
     *
     * \param[in] __Point2D<T> &b
     */
    __Point2D<T> & operator=(const __Point2D<T> &b)
    {
      x = b.x;
      y = b.y;
      return *this;
    };

    /** \brief Copy operator with single value.
     *
     * Copies the values of scala b to a:
     \verbatim
     a = value; \endverbatim
     * which is equivalent to
      \verbatim
     a.x = a.y = value; \endverbatim
     *
     * \param[in] T value
     */
    __Point2D<T> & operator=(const T value)
    {
      x = value;
      y = value;
      return *this;
    };

    /** \brief Copy operator with pointer to an array of length three.
     *
     * Copies the values of the array b of length 3 to a:
      \verbatim
     a = b; \endverbatim
     * which is equivalent to
      \verbatim
     a.x = b[0];
     a.y = b[1]; \endverbatim
     *
     * \param[in] __Point2D<T> &b
     */
    __Point2D<T> & operator=(const T* values)
    {
      x = values[0];
      y = values[1];
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
     a.x *= b;
     a.y *= b; \endverbatim
     *
     * \param[in] T b
     */
    __Point2D<T> & operator*=(const T value)
    {
      x *= value;
      y *= value;
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
     a.x /= b;
     a.y /= b; \endverbatim
     *
     * \param[in] T b
     */
    __Point2D<T> & operator/=(const T value)
    {
      x /= value;
      y /= value;
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
     a.x += b.x;
     a.y += b.y; \endverbatim
     *
     * \param[in] __Point2D<T> b
     */
    __Point2D<T> & operator+=(const __Point2D<T> &a)
    {
      x += a.x;
      y += a.y;
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
     a.x -= b.x;
     a.y -= b.y; \endverbatim
     *
     * \param[in] __Point2D<T> b
     */
    __Point2D<T> & operator-=(const __Point2D<T> &b)
    {
      x = x - b.x;
      y = y - b.y;
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
     c.x = a.x + b.x;
     c.y = a.y + b.y; \endverbatim
     *
     * \param[in] __Point2D<T> b
     */
    const __Point2D<T> operator+(const __Point2D<T> &b) const
    {
      __Point2D<T> r = *this;
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
     c.x = a.x - b.x;
     c.y = a.y - b.y; \endverbatim
     *
     * \param[in] __Point2D<T> b
     */
    const __Point2D<T> operator-(const __Point2D<T> &b) const
    {
      __Point2D<T> r = *this;
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
     c.x = a.x * b;
     c.y = a.y * b; \endverbatim
     *
     * \param[in] T value
     */
    const __Point2D<T> operator*(const T value) const
    {
      __Point2D<T> r = *this;
      r *= value;
      return r;
    };

    void normalise()
    {
      double l = sqrt(x * x + y * y);
      x /= l;
      y /= l;
    }

    double length()
    {
      return sqrt(x * x + y * y);
    }

    bool operator==(const __Point2D<T> a)
    {
      return (x == a.x) && (y == a.y);
    }

    /** \brief Debug function.
     *
     * Gives the values as an input to an ostream:
     *
     \verbatim
     __Point2D<double> p;
     cout << p << endl; \endverbatim
     * which gives
     \verbatim
     x, y \endverbatim
     * \param[in] std::ostream stream
     * \param[in] __Point2D<T> p
     *
     */
    friend std::ostream& operator<<(std::ostream& str, const __Point2D<T>& p)
    {
      str << p.x << ", " << p.y;
      return str;
    };

};

/** Pre-defined standard __Point2D class with double values */
typedef __Point2D<double> P2D;

#endif // __POINT_2D_H__


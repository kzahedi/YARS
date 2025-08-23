#ifndef __POINT_3D_H__
#define __POINT_3D_H__

#include <yars/defines/types.h>

#include <ostream>
#include <math.h>

namespace yars {

/** \brief This class implements a three dimensional point.
 *
 * Point values are accessed directly by x, y, z:
 *
 *
 \verbatim
 P3D p;
 p.x;
 p.y;
 p.z; \endverbatim
 *
 * P3D is pre-defined to __Point3D<double>
 *
 * \todo Unit test
 */
class P3D
{
  public:
    /** \brief x-value of the three dimension point.
     */
    double x;
    /** \brief y-value of the three dimension point.
     */
    double y;
    /** \brief z-value of the three dimension point.
     */
    double z;
    /** \brief Standard constructor.
     *
     * Sets x, y, z to 0.0
     */
    P3D();

    /** \brief Copy constructor with pointer to array.
     *
     * Takes an array of length 3 and sets the point values accordingly:
     \verbatim
     x = p[0];
     y = p[1];
     z = p[2]; \endverbatim
     *
     * Note that this function is not fault tolerant, as the length of the array
     * cannot be checked.
     * \param[in] double* pointer, pointer to the array of length 3
     */
    P3D(const double* p);


    /** \brief Copy constructor with the values.
     *
     * Takes three values and sets the point accordingly.
      \verbatim
     x = _x;
     y = _y;
     z = _z; \endverbatim
     *
     * \param[in] double _x, x-value
     * \param[in] double _y, y-value
     * \param[in] double _z, z-value
     */
    P3D(const double _x, const double _y, const double _z);

    /** \brief Copy constructor.
     *
     * Takes a P3D and copies its values.
      \verbatim
     x = p.x;
     y = p.y;
     z = p.z; \endverbatim
     *
     * \param[in] P3D *p
     */
    P3D(const P3D &p);

    /** \brief Copy operator.
     *
     * Copies the values of point b to a:
      \verbatim
     a = b; \endverbatim
     *
     * \param[in] P3D &b
     */
    P3D & operator=(const P3D &b);

    /** \brief Copy operator with single value.
     *
     * Copies the values of scala b to a:
     \verbatim
     a = value; \endverbatim
     * which is equivalent to
      \verbatim
     a.x = a.y = a.z = value; \endverbatim
     *
     * \param[in] double value
     */
    P3D & operator=(const double value);
    P3D & operator=(const float  value);
    P3D & operator=(const int    value);


    /** \brief Copy operator with pointer to an array of length three.
     *
     * Copies the values of the array b of length 3 to a:
      \verbatim
     a = b; \endverbatim
     * which is equivalent to
      \verbatim
     a.x = b[0];
     a.y = b[1];
     a.z = b[2]; \endverbatim
     *
     * \param[in] P3D &b
     */
    P3D & operator=(const double* values);

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
     a.y *= b;
     a.z *= b; \endverbatim
     *
     * \param[in] double b
     */
    P3D & operator*=(const double value);


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
     a.y /= b;
     a.z /= b; \endverbatim
     *
     * \param[in] double b
     */
    P3D & operator/=(const double value);


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
     a.y += b.y;
     a.z += b.z; \endverbatim
     *
     * \param[in] P3D b
     */
    P3D & operator+=(const P3D &a);

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
     a.y += b.y;
     a.z += b.z; \endverbatim
     *
     * \param[in] P3D b
     */
    P3D & operator+=(double a);


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
     a.y -= b.y;
     a.z -= b.z; \endverbatim
     *
     * \param[in] P3D b
     */
    P3D & operator-=(const P3D &b);

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
     c.y = a.y + b.y;
     c.z = a.z + b.z; \endverbatim
     *
     * \param[in] P3D b
     */
    const P3D operator+(const P3D &b) const;

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
     c.y = a.y - b.y;
     c.z = a.z - b.z; \endverbatim
     *
     * \param[in] P3D b
     */
    const P3D operator-(const P3D &b) const;

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
     c.y = a.y * b;
     c.z = a.z * b; \endverbatim
     *
     * \param[in] double value
     */
    const P3D operator*(const double value) const;
    /* cross product */
    const P3D operator*(const P3D &b) const;

    /* dot product */
    double dot(const P3D &b) const;


    void normalise();

    double length();

    bool operator==(const P3D a);

    bool operator!=(const P3D a);

    double dist(P3D p);

    /** \brief Debug function.
     *
     * Gives the values as an input to an ostream:
     *
     \verbatim
     __Point3D<double> p;
     cout << p << endl; \endverbatim
     * which gives
     \verbatim
     x, y, z \endverbatim
     * \param[in] std::ostream stream
     * \param[in] P3D p
     *
     */
    friend std::ostream& operator<<(std::ostream& str, const P3D& p)
    {
      str << p.x << ", " << p.y << ", " << p.z;
      return str;
    };

    void invert();
    void rotate(P3D p);
    void rotateInv(P3D p);


};

} // namespace yars

using yars::P3D;

#endif // __POINT_3D_H__

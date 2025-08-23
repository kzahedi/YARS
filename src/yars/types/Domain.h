#ifndef __DOMAIN_H__
#define __DOMAIN_H__

#include <yars/defines/types.h>

#include <math.h>
#include <ostream>
#include <iostream>



/** \brief Data type that defines a domain range for a single value.
 * \todo Unit test
 */
template<class T>
class __Domain
{
  public:
    /** \brief Minimum value of the domain.
     *
     *  Lower bound.
     **/
    T min;

    /** \brief Maximal value of the domain.
     *
     *  Upper bound.
     **/
    T max;

    /** \brief Standard constructor.
     *
     * Sets the minimal and maximal value to zero.
     **/
    __Domain<T>()
    {
      min = 0;
      max = 0;
    };

    /** \brief Copy constructor for pointer for a value of length two.
     *
     * Sets the minimal value to the first value of the array, and the maximal
     * value to the second.
     * \param[in] pointer to T array
     **/
    __Domain<T>(const T* p)
    {
      min = p[0];
      max = p[1];
    };

    /** \brief Set constructor taking two value.
     *
     * Sets the minimal value to the first value, and the maximal
     * value to the second.
     * \param[in] T _min
     * \param[in] T _max
     **/
    __Domain<T>(const T _min, const T _max)
    {
      min = _min;
      max = _max;
    };


    /** \brief Copy constructor.
     *
     * Copies the values of the given __Domain<T>
     **/
    __Domain<T>(const __Domain<T> *p)
    {
      min = p->min;
      max = p->max;
    };

    /** \brief Copy constructor (by reference).
     *
     * Copies the values of the given __Domain<T>
     **/
    __Domain<T>(const __Domain<T> &d)
    {
      min = d.min;
      max = d.max;
    };

    /** \brief Returns the width of the domain.
     *
     * \return (max - min)
     **/
    T width()
    {
      return fabs(max - min);
    }

    /** \brief Maps a value within the domain [min, max] to the domain [0, 1]
     *
     * \return (value - min) / (max - min)
     **/
    T map(T value)
    {
      return (cut(value) - min) / (max - min);
    }

    /** \brief Maps a value within the domain [0, 1] to the domain [min, max]
     *
     * \return (value - min) / (max - min)
     **/
    T remap(T value)
    {
      return cut(value * (max - min) + min);
    }

    /** \brief Cuts a value to the domain range. If the value is larger than
     * maximum, then it is set to maximum. If it is smaller than the minimum, it is set
     * to the minimum.
     *
     * \param[in]  T value
     * \param[out] T value, cut to the range
     **/
    void cut(T *value)
    {
      if(max > min)
      {
        if(*value > max) *value = max;
        if(*value < min) *value = min;
      }
      else
      {
        if(*value < max) *value = max;
        if(*value > min) *value = min;
      }
    }

    T cut(T value)
    {
      if(max > min)
      {
        if(value > max) value = max;
        if(value < min) value = min;
      }
      else
      {
        if(value < max) value = max;
        if(value > min) value = min;
      }
      return value;
    }

    /** \brief Copy operator.
     *
     * a = b, the values of a are so to b
     *
     * \param[in]  __Domain<T> d
     **/
    __Domain<T> & operator=(const __Domain<T> &d)
    {
      min = d.min;
      max = d.max;
      return *this;
    };

    /** \brief Copy operator.
     *
     * a = b, both values of a (min, max) are so to b
     *
     * \param[in]  T value
     **/
    __Domain<T> & operator=(const T value)
    {
      min = value;
      max = value;
      return *this;
    };

    /** \brief Copy operator.
     *
     * a = b. Value of a are set to b[0] and b[1]
     *
     * \param[in]  T* values
     **/
    __Domain<T> & operator=(const T* values)
    {
      min = values[0];
      max = values[1];
      return *this;
    };

    /** \brief Scaling operator.
     *
     * a *= b. Values of a are both scaled by value
     *
     * \param[in]  T value
     **/
    __Domain<T> & operator*=(const T value)
    {
      min *= value;
      max *= value;
      return *this;
    };

    /** \brief Scaling operator.
     *
     * a /= b. Values of a are both scaled by 1/value
     *
     * \param[in] T value
     **/
    __Domain<T> & operator/=(const T value)
    {
      min /= value;
      max /= value;
      return *this;
    };

    /** \brief Addition operator.
     *
     * a += b. Values of a are increased b by, such that a.min += b.min and
     * a.max += b.max
     *
     * \param[in]  __Domain<T> value
     **/
    __Domain<T> & operator+=(const __Domain<T> &d)
    {
      min += d.min;
      max += d.max;
      return *this;
    };

    /** \brief Subtraction operator.
     *
     * a -= b. Values of a are decreased b by, such that a.min -= b.min and
     * a.max -= b.max
     *
     * \param[in]  __Domain<T> value
     **/
    __Domain<T> & operator-=(const __Domain<T> &d)
    {
      min = min - d.min;
      max = max - d.max;
      return *this;
    };

    /** \brief Addition operator.
     *
     * c = a + b.
     *
     * \param[in]  __Domain<T> value
     **/
    const __Domain<T> operator+(const __Domain<T> &d) const
    {
      __Domain<T> r = *this;
      r += d;
      return r;
    };

    /** \brief Substraction operator.
     *
     * c = a - b.
     *
     * \param[in]  __Domain<T> value
     **/
    const __Domain<T> operator-(const __Domain<T> &d) const
    {
      __Domain<T> r = *this;
      r -= d;
      return r;
    };

    /** \brief Multiplication operator.
     *
     * c = a * b, where b is a scalar.
     *
     * \param[in]  T value
     **/
    const __Domain<T> operator*(const T value) const
    {
      __Domain<T> r = *this;
      r *= value;
      return r;
    };

    /** \brief Debug method.
     *
     * To be used with std::ostream, e.g. oss << d << endl;
     *
     **/
    friend std::ostream& operator<<(std::ostream& str, const __Domain<T>& p)
    {
      str << "[" << p.min << ", " << p.max << "]";
      return str;
    };

};

typedef __Domain<double> Domain;

#endif // __DOMAIN_H__


#ifndef __COLOR_H__
#define __COLOR_H__

#include <yars/defines/types.h>
#include <yars/util/YarsErrorHandler.h>

#include <string>

#ifdef USE_THREAD_SAFE
#  include "pthread.h"
#endif // USE_THREAD_SAFE

/** \brief Implements thread-safe access to color.
 *
 * This class implements thread-safe access to RGBA color values.
 *
 * Values are in the unit interval.
 *
 * \todo Unit test
 */
class Colour
{
  public:
    /** \brief Standard constructor.
     *
     * RGB values are set to 0, alpha to 1.
     */
    Colour();
    /** \brief Standard value constructor.
     *
     * Takes RGBA values. No standard values defined, i.e. every must be given
     * to the constructor.
     *
     * \param[in] red
     * \param[in] green
     * \param[in] blue
     * \param[in] alpha
     */
    Colour(double red, double green, double blue, double alpha);
    Colour(float red, float green, float blue, float alpha);

    /** \brief Returns the red-value.
     *
     * Values are in the unit interval.
     *
     * \param[out] red, double
     */
    double red();
    /** \brief Returns the green-value.
     *
     * Values are in the unit interval.
     *
     * \param[out] green, double
     */
    double green();
    /** \brief Returns the blue-value.
     *
     * Values are in the unit interval.
     *
     * \param[out] blue, double
     */
    double blue();
    /** \brief Returns the alpha-value.
     *
     * Values are in the unit interval.
     *
     * \param[out] double alpha
     */
    double alpha();

    /** \brief Sets the red-value.
     *
     * Value has to be in the unit interval.
     *
     * \param[out] red, double
     */
    void setRed(double red);
    void setRed(float red);
    /** \brief Sets the green-value.
     *
     * Value has to be in the unit interval.
     *
     * \param[in] green, double
     * \exception YarsException with detailed message about the cause.
     */
    void setGreen(double green);
    void setGreen(float green);
    /** \brief Sets the blue-value.
     *
     * Value has to be in the unit interval.
     *
     * \param[in] blue, double
     */
    void setBlue(double blue);
    void setBlue(float blue);
    /** \brief Sets the alpha-value.
     *
     * Value has to be in the unit interval.
     *
     * \param[in] alpha, double
     * \exception YarsException with detailed message about the cause.
     */
    void setAlpha(double alpha);
    void setAlpha(float alpha);

    /** \brief Copy operator.
     *
     * Copied the values from c to a:
     \verbatim
     a = c; \endverbatim
     *
     * \param[in] c, Colour
     */
    Colour& operator= (const Colour &c);
    /** \brief Inverse RGB value.
     *
     * Inverses the RGB values (not the A value) of the color. This is
     * equivalent to:
     \verbatim
     a.setRed(1 - a.getRed());
     a.setBlue(1 - a.getBlue());
     a.setGreen(1 - a.getGreen()); \endverbatim
     *
     */
    void   inverse();

    /** \brief Pre-defined color black.
     * Equivalent to Colour(0.0, 0.0, 0.0, 1.0)
     */
    static Colour BLACK;
    /** \brief Pre-defined color blue.
     * Equivalent to Colour(0.0, 0.0, 1.0, 1.0)
     */
    static Colour BLUE;
    /** \brief Pre-defined color yellow.
     * Equivalent to Colour(1.0, 0.0, 1.0, 1.0)
     */
    static Colour YELLOW;

    /** \brief Pre-defined color red.
     * Equivalent to Colour(1.0, 0.0, 0.0, 1.0)
     */
    static Colour RED;

    static Colour WHITE;

    friend std::ostream& operator<<(std::ostream& str, Colour &c)
    {
      str << c.red() << ", " << c.green() << ", " << c.blue() << ", " << c.alpha();
      return str;
    };

  private:
    void __init(double red, double green, double blue, double alpha);
    void __check(std::string name, double value);
    void __setRed(double red);
    void __setGreen(double green);
    void __setBlue(double blue);
    void __setAlpha(double alpha);

    double _red;
    double _green;
    double _blue;
    double _alpha;
#ifdef USE_THREAD_SAFE
    pthread_mutex_t _colorMutex;
#endif // USE_THREAD_SAFE

};


#endif // __COLOR_H__

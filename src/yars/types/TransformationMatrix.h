#ifndef __TRANSFORMATION_MATRIX_H__
#define __TRANSFORMATION_MATRIX_H__

#include "Matrix.h"

#define TM_XYZ 10012
#define TM_ZYX 10013

class TransformationMatrix : public Matrix
{
  public:
    TransformationMatrix();
    TransformationMatrix(double alpha, double beta, double gamma, double x, double y, double z, int mode = TM_XYZ);
    /* set rotation matrix */
    TransformationMatrix(double alpha, double beta, double gamma, int mode = TM_XYZ);
    TransformationMatrix(TransformationMatrix &t);

    void   setRotation(double alpha, double beta, double gamma, int mode = TM_XYZ);
    void   setTranformation(double alpha, double beta, double gamma, double x, double y, double z, int mode = TM_XYZ);
    void   setTranslation(double x, double y, double z);

  private:
    void __checkMode(int mode);
    void __setXYZ(double alpha, double beta, double gamma, double x, double y, double z);
    void __setZYX(double alpha, double beta, double gamma, double x, double y, double z);
};

#endif // __TRANSFORMATION_MATRIX_H__

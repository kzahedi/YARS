#ifndef __TRANSFORMATION_MATRIX_H__
#define __TRANSFORMATION_MATRIX_H__

#include "Matrix.h"

#define TM_XYZ 10012
#define TM_ZYX 10013

class TransformationMatrix : public Matrix
{
  public:
    TransformationMatrix();
    TransformationMatrix(yReal alpha, yReal beta, yReal gamma, yReal x, yReal y, yReal z, int mode = TM_XYZ);
    /* set rotation matrix */
    TransformationMatrix(yReal alpha, yReal beta, yReal gamma, int mode = TM_XYZ);
    TransformationMatrix(TransformationMatrix &t);

    void   setRotation(yReal alpha, yReal beta, yReal gamma, int mode = TM_XYZ);
    void   setTranformation(yReal alpha, yReal beta, yReal gamma, yReal x, yReal y, yReal z, int mode = TM_XYZ);
    void   setTranslation(yReal x, yReal y, yReal z);

  private:
    void __checkMode(int mode);
    void __setXYZ(yReal alpha, yReal beta, yReal gamma, yReal x, yReal y, yReal z);
    void __setZYX(yReal alpha, yReal beta, yReal gamma, yReal x, yReal y, yReal z);
};

#endif // __TRANSFORMATION_MATRIX_H__

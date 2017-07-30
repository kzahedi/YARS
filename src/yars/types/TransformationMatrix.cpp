#include "TransformationMatrix.h"

#include <math.h>

TransformationMatrix::TransformationMatrix()
  : Matrix(4, 4, 0.0)
{

}

TransformationMatrix::TransformationMatrix(TransformationMatrix &t)
  : Matrix(4, 4, 0.0)
{
  for(int r = 0; r < 4; r++)
  {
    for(int c = 0; c < 4; c++)
    {
      __set(r, c, t(r,c));
    }
  }
}

TransformationMatrix::TransformationMatrix(double x, double y, double z, int mode)
{
   setRotation(x, y, z, mode);
}

TransformationMatrix::TransformationMatrix(double alpha, double beta, double gamma, double x, double y, double z, int mode)
{
  // CHECKED 01.07.11
  setTranformation(alpha, beta, gamma, x, y, z, mode);
}


void TransformationMatrix::__checkMode(int mode)
{
  if(mode != TM_XYZ && mode != TM_ZYX)
  {
    // TODO: exception
    cerr << "ERROR: TransformationMatrix: unknow mode" << endl;
  }
}

void TransformationMatrix::setRotation(double alpha, double beta, double gamma, int mode)
{
  // CHECKED 01.07.11
  setTranformation(alpha, beta, gamma, 0, 0, 0, mode);
}

void TransformationMatrix::setTranslation(double x, double y, double z)
{
  // CHECKED 01.07.11
  setTranformation(0, 0, 0, x, y, z);
}

void TransformationMatrix::setTranformation(double alpha, double beta, double gamma, double x, double y, double z, int mode)
{
  __checkMode(mode);
  __init(4, 4, 0.0);
  if(mode == TM_XYZ) __setXYZ(alpha, beta, gamma, x, y, z);
  if(mode == TM_ZYX) __setZYX(alpha, beta, gamma, x, y, z);
}

void TransformationMatrix::__setXYZ(double alpha, double beta, double gamma, double x, double y, double z)
{
  // CHECKED 01.07.11
  double sa = sin(alpha); double ca = cos(alpha);
  double sb = sin(beta);  double cb = cos(beta);
  double sg = sin(gamma); double cg = cos(gamma);


  // first row
  __set(0, 0, cb * cg);
  __set(0, 1, cg * sa * sb - ca * sg);
  __set(0, 2, ca * cg * sb + sa * sg);
  __set(0, 3, x);

  // second row
  __set(1, 0, cb * sg);
  __set(1, 1, ca * cg + sa * sb * sg);
  __set(1, 2, -cg * sa + ca * sb * sg);
  __set(1, 3, y);

  // third row
  __set(2, 0, -sb);
  __set(2, 1, cb * sa);
  __set(2, 2, ca * cb);
  __set(2, 3, z);

  // forth row
  __set(3, 0, 0.0);
  __set(3, 1, 0.0);
  __set(3, 2, 0.0);
  __set(3, 3, 1.0);
}

void TransformationMatrix::__setZYX(double alpha, double beta, double gamma, double x, double y, double z)
{
  double sa = sin(alpha); double ca = cos(alpha);
  double sb = sin(beta);  double cb = cos(beta);
  double sg = sin(gamma); double cg = cos(gamma);

  // first row
  __set(0, 0, cb * cg);
  __set(0, 1, -cb * sg);
  __set(0, 2, sb);
  __set(0, 3, x);

  // second row
  __set(1, 0, cg * sa * sb + ca * sg);
  __set(1, 1, ca * cg - sa * sb * sg);
  __set(1, 2, -cb * sa);
  __set(1, 3, y);

  // third row
  __set(2, 0, -ca * cg * sb + sa * sg);
  __set(2, 1, cg * sa + ca * sb * sg);
  __set(2, 2, ca * cb);
  __set(2, 3, z);

  // forth row
  __set(3, 0, 0.0);
  __set(3, 1, 0.0);
  __set(3, 2, 0.0);
  __set(3, 3, 1.0);
}

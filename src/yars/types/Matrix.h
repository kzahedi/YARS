#ifndef __MIS_MATRIX_H__
#define __MIS_MATRIX_H__

#ifdef _MSC_VER
#  pragma warning( disable : 4290 )
#endif

#include <yars/defines/types.h>
#include <yars/util/YarsErrorHandler.h>

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Matrix
{
  public:
    /** Most simple constructor. Every cell is initialised with 0.*/
    Matrix();
#ifndef _MSC_VER
    Matrix(Matrix &m);
#endif // _MSC_VER
    Matrix(const Matrix &m);
    Matrix(int rows, int cols);
    /** Most simple constructor. Every cell is initialised with the given
     * value.*/
    Matrix(int rows, int cols, double initialValue);
    /** Most simple constructor. The cells are initialised with the values of
     * the vector. If the vector is smaller than the numbers of cells, the
     * remaining cells are filled with 0. The filling occurs rows first.
     */
    Matrix(int rows, int cols, std::vector<double> initialValues);
    /** Destructor. */
    ~Matrix();

    /** Allows to access and modify the values by indexing */
    double& operator()(int row, int col);
    double  operator()(int row, int col) const;
    Matrix& operator+=(const Matrix &m);
    Matrix& operator-=(const Matrix &m);

    // TODO: Matrix A(10,10); Matrix A = B; does not work
    const Matrix operator* (const double factor);
    Matrix&      operator*=(const double factor);
    const Matrix operator* (const Matrix &m);

    Matrix&      operator= (const Matrix &m);
    Matrix&      operator*=(const Matrix &m);

    Matrix&      operator= (const double d);
    Matrix&      operator-=(const double d);
    Matrix&      operator/=(const double d);

    const Matrix operator-(const Matrix &m);
    const Matrix operator+(const Matrix &m);

    int    cols() const;
    int    rows() const;

    void   setDiagonalMatrix(double value);
    void   setDiagonalMatrix(float value);

    double rowSum(const int row);
    double colSum(const int col);

    double L2();

    void   reset(int rows, int cols, double value = 0.0);
    void   rescaleRows(double value, bool verbose);

    double det();
    void   inverse();
    void   transpose();
    void   adjunct();
    void   cut(int r_index = -1, int c_index = -1);

    friend ostream& operator<<(ostream& str, const Matrix& m)
    {
      for(int r = 0; r < m.rows(); r++)
      {
        for(int c = 0; c < m.cols(); c++)
        {
          str << m(r,c) << " ";
        }
        str << endl;
      }
      return str;
    };

  protected:
    void   __set(const int row, const int col, const double value);
    double __get(const int row, const int col);
    void   __deleteCells();
    void __init(const int rows, const int cols, double initialValue);

  private:
    void __setDiagonalMatrix(double value);
    void __copy(const Matrix &m);
    void __check(int row, int col);
    int _rows;
    int _cols;
    double **_cell;

};
#endif // __MIS_MATRIX_H__

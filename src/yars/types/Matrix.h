#ifndef __MIS_MATRIX_H__
#define __MIS_MATRIX_H__

#ifdef _MSC_VER
#  pragma warning( disable : 4290 )
#endif

#include <yars/defines/types.h>

#include <vector>
#include <string>
#include <iostream>

class MatrixException : public std::exception
{
  public:
    explicit MatrixException(const std::string& what)
      :
        m_what(what)
  {}

    virtual ~MatrixException() noexcept {}

    virtual const char * what() const noexcept
    {
      return m_what.c_str();
    }

    virtual void message() const noexcept
    {
      std::cout << "MatrixException: " << m_what << std::endl;
    }


  private:
    std::string m_what;
};

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
    Matrix(int rows, int cols, yReal initialValue);
    /** Most simple constructor. The cells are initialised with the values of
     * the vector. If the vector is smaller than the numbers of cells, the
     * remaining cells are filled with 0. The filling occurs rows first.
     */
    Matrix(int rows, int cols, std::vector<yReal> initialValues);
    /** Destructor. */
    ~Matrix();

    /** Allows to access and modify the values by indexing */
    yReal& operator()(int row, int col) noexcept(false);
    yReal  operator()(int row, int col) const noexcept(false);
    Matrix& operator+=(const Matrix &m)  noexcept(false);
    Matrix& operator-=(const Matrix &m)  noexcept(false);

    // TODO: Matrix A(10,10); Matrix A = B; does not work
    const Matrix operator* (const yReal factor);
    Matrix&      operator*=(const yReal factor);
    const Matrix operator* (const Matrix &m);

    Matrix&      operator= (const Matrix &m);
    Matrix&      operator*=(const Matrix &m);

    Matrix&      operator= (const yReal d);
    Matrix&      operator-=(const yReal d);
    Matrix&      operator/=(const yReal d);

    const Matrix operator-(const Matrix &m);
    const Matrix operator+(const Matrix &m);

    int    cols() const;
    int    rows() const;

    void   setDiagonalMatrix(double value);
    void   setDiagonalMatrix(float value);

    yReal rowSum(const int row);
    yReal colSum(const int col);

    yReal L2();

    void   reset(int rows, int cols, yReal value = 0.0);
    void   rescaleRows(yReal value, bool verbose);

    yReal det() noexcept(false);
    void   inverse();
    void   transpose();
    void   adjunct();
    void   cut(int r_index = -1, int c_index = -1);

    friend std::ostream& operator<<(std::ostream& str, const Matrix& m)
    {
      for(int r = 0; r < m.rows(); r++)
      {
        for(int c = 0; c < m.cols(); c++)
        {
          str << m(r,c) << " ";
        }
        str << std::endl;
      }
      return str;
    };

  protected:
    void   __set(const int row, const int col, const yReal value);
    yReal __get(const int row, const int col);
    void   __deleteCells();
    void __init(const int rows, const int cols, yReal initialValue);

  private:
    void __setDiagonalMatrix(yReal value);
    void __copy(const Matrix &m);
    void __check(int row, int col) noexcept(false);
    int _rows;
    int _cols;
    yReal **_cell;

};
#endif // __MIS_MATRIX_H__

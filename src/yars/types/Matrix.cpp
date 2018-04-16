#include "Matrix.h"

#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

Matrix::Matrix()
{
  _cell = NULL;
  _rows = 0;
  _cols = 0;
}

#ifndef _MSC_VER
Matrix::Matrix(Matrix &m)
{
  _cell = NULL;
  __init(m.rows(), m.cols(), 0.0);
  __copy(m);
}
#endif // _MSC_VER

Matrix::Matrix(const Matrix &m)
{
  _cell = NULL;
  __init(m.rows(), m.cols(), 0.0);
  __copy(m);
}

Matrix::Matrix(int rows, int cols)
{
  _cell = NULL;
  __init(rows,cols,0.0); // standard matrix
}

Matrix::Matrix(int rows, int cols, double initialValue)
{
  _cell = NULL;
  __init(rows,cols,initialValue); // standard matrix
}

Matrix::Matrix(int rows, int cols, std::vector<double> initialValues)
{
  _cell = NULL;
  __init(rows,cols, 0.0);
  for(int i = 0; i < _rows; i++)
  {
    for(int j = 0; j < _cols; j++)
    {
      if((unsigned int)(i*_rows + j) > initialValues.size())
      {
        _cell[i][j] = 0.0;
      }
      else
      {
        _cell[i][j] = initialValues[i*_rows + j];
      }
    }
  }
}

Matrix::~Matrix()
{
  __deleteCells();
}

void Matrix::__init(const int rows, const int cols, double initialValue)
{
  if(_cell != NULL)
  {
    __deleteCells();
  }

  _rows = rows;
  _cols = cols;

  _cell = new double* [rows];
  for(int i=0; i<rows; i++)
  {
    _cell[i] = new double [cols];
  }

  for(int i = 0; i < rows; i++)
  {
    for(int j = 0; j < cols; j++)
    {
      _cell[i][j] = initialValue;
    }
  }
}


double Matrix::operator()(int row, int col) const
{
  if(row >= _rows || col >= _cols || row < 0 || col < 0)
  {
    std::stringstream oss;
    oss << "Invalid matrix index: ";
    if(row > _rows)
    {
      oss << "row too large: " << row << " > " << _rows;
    }
    if(row < 0)
    {
      oss << "row is negative: " << row;
    }
    if(col > _cols)
    {
      oss << "col too large: " << col << " > " << _cols;
    }
    if(col < 0)
    {
      oss << "col is negative: " << col;
    }
    YarsErrorHandler::push(oss.str());
  }
  return _cell[row][col];
}

double& Matrix::operator()(int row, int col)
{
  __check(row,col);
  return _cell[row][col];
}

void Matrix::__set(const int row, const int col, const double value)
{
  __check(row,col);
  _cell[row][col] = value;
}

double Matrix::__get(int row, int col)
{
  __check(row,col);
  return _cell[row][col];
}

double Matrix::rowSum(int row)
{
  double sum = 0;
  for(int c = 0; c < _cols; c++)
  {
    sum += _cell[row][c];
  }
  return sum;
}

double Matrix::colSum(int col)
{
  double sum = 0;
  for(int r = 0; r < _rows; r++)
  {
    sum += _cell[r][col];
  }
  return sum;
}

int Matrix::cols() const
{
  return _cols;
}

int Matrix::rows() const
{
  return _rows;
}

void Matrix::__check(int row, int col)
{
  if(row >= _rows || col >= _cols || row < 0 || col < 0)
  {
    std::stringstream oss;
    oss << "Invalid matrix index: ";
    if(row >= _rows)
    {
      oss << "row too large: " << row << " >= " << _rows;
    }
    if(row < 0)
    {
      oss << "row is negative: " << row;
    }
    if(col >= _cols)
    {
      oss << "col too large: " << col << " >= " << _cols;
    }
    if(col < 0)
    {
      oss << "col is negative: " << col;
    }
    YarsErrorHandler::push(oss.str());
  }
}

Matrix& Matrix::operator+=(const Matrix &m)
{
  if(_rows != m.rows() || _cols != m.cols())
  {
    std::stringstream oss;
    oss << "Cannot += matrix, because ";
    if(_rows != m.rows())
    {
      oss << "number of rows do not match: " << _rows << " != " << m.rows();
    }
    if(_cols != m.cols())
    {
      oss << "number of cols do not match: " << _cols << " != " << m.cols();
    }
    YarsErrorHandler::push(oss.str());
  }

  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, __get(r,c) + m(r,c));
    }
  }
  return *this;
}

Matrix& Matrix::operator-=(const Matrix &m)
{
  if(_rows != m.rows() || _cols != m.cols())
  {
    std::stringstream oss;
    oss << "Cannot -= matrix, because ";
    if(_rows != m.rows())
    {
      oss << "number of rows do not match: " << _rows << " != " << m.rows();
    }
    if(_cols != m.cols())
    {
      oss << "number of cols do not match: " << _cols << " != " << m.cols();
    }
    YarsErrorHandler::push(oss.str());
  }

  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, __get(r,c) - m(r,c));
    }
  }
  return *this;
}

Matrix& Matrix::operator*=(const double factor)
{
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, __get(r, c) * factor);
    }
  }
  return *this;
}

const Matrix Matrix::operator+(const Matrix &m)
{
  Matrix r = *this;
  r += m;
  return r;
}

const Matrix Matrix::operator-(const Matrix &m)
{
  Matrix r = *this;
  r -= m;
  return r;
}

const Matrix Matrix::operator*(const double factor)
{
  Matrix r = *this;
  r *= factor;
  return r;
}

const Matrix Matrix::operator*(const Matrix &m)
{
  Matrix c(m.rows(), m.cols());
  c = *this;
  c *= m;
  return c;
}

Matrix& Matrix::operator=(const Matrix &m)
{
  if(this == &m) return *this;


  __init(m.rows(), m.cols(), 0.0);
  __copy(m);
  return *this;
}

void Matrix::__deleteCells()
{
  if (_cell != NULL)
  {
    for(int i=_rows-1; i>=0; i--)
    {
      delete [] _cell[i];
    }
    delete [] _cell;
  }
  _cell = NULL;
}

void Matrix::__copy(const Matrix &m)
{
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, m(r,c));
    }
  }
}

void Matrix::setDiagonalMatrix(double value)
{
  __setDiagonalMatrix(value);
}
void Matrix::setDiagonalMatrix(float value)
{
  __setDiagonalMatrix(value);
}

void Matrix::__setDiagonalMatrix(double value)
{
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      if(r == c)
      {
        __set(r, c, value);
      }
      else
      {
        __set(r, c, 0.0);
      }
    }
  }
}

void Matrix::reset(int rows, int cols, double value)
{
  __init(rows, cols, value);
}

Matrix& Matrix::operator=(const double d)
{
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, d);
    }
  }
  return *this;
}

Matrix& Matrix::operator-=(const double d)
{
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, __get(r,c) - d);
    }
  }
  return *this;
}

Matrix& Matrix::operator/=(const double d)
{
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      __set(r, c, __get(r,c) / d);
    }
  }
  return *this;
}


void Matrix::rescaleRows(double value, bool verbose)
{
  for(int r = 0; r < _rows; r++)
  {
    double sum = 0;
    for(int c = 0; c < _cols; c++)
    {
      sum += __get(r,c);
    }
    if(sum != value)
    {
      if(verbose)
      {
        std::cout << "row sum of row " << r << " is " << sum << std::endl;
      }
      sum /= value;
      for(int c = 0; c < _cols; c++)
      {
        __set(r, c, __get(r,c) / sum);
      }
      if(verbose)
      {
        sum = 0;
        for(int c = 0; c < _cols; c++)
        {
          sum += __get(r,c);
        }
        std::cout << "row sum of row " << r << " is now " << sum << std::endl;
      }
    }
  }
}

double Matrix::L2()
{
  double d = 0.0;
  double v = 0.0;
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      v = __get(r,c);
      d += v * v;
    }
  }
  return sqrt(d);
}

double Matrix::det()
{
  if(cols() != rows())
  {
    YarsErrorHandler::push("Determinant is only implemented for quadratic matrices");
  }

  double result = 0;

  if(_rows == 1)
  {
    result = __get(0,0);
    return result;
  }

  if(_rows == 2)
  {
    result = __get(0, 0) * __get(1, 1) - __get(0, 1) * __get(1, 0);
    return result;
  }

  for(int i = 0; i < cols(); i++)
  {
    Matrix tmp(_rows-1, _cols-1, 0.0);
    for(int j = 1; j < rows(); j++)
    {
      for(int k = 0; k < cols(); k++)
      {
        if(k < i)
        {
          tmp(j - 1, k) = __get(j,k);
        }
        else if(k > i)
        {
          tmp(j - 1, k - 1) = __get(j,k);
        }
      }
    }
    result += __get(0,i) * pow(-1, (double)i) * tmp.det();
  }
  return result;
}

void Matrix::transpose()
{
  Matrix m = (*this);
  reset(m.cols(), m.rows());

  for(int r = 0; r < m.rows(); r++)
  {
    for(int c = 0; c < m.cols(); c++)
    {
      __set(c,r, m(r,c));
    }
  }
}

void Matrix::adjunct()
{
  Matrix m(_rows, _cols);

  m = *this;

  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      Matrix ij = m;
      ij.cut(r,c);
      double v = pow(-1, (double)((r+1)+(c+1))) * ij.det();
      __set(r, c, v);
    }
  }

  transpose();

}

void Matrix::cut(int r_index, int c_index)
{
  int r_new = _rows;
  int c_new = _cols;
  if(r_index >= 0)
  {
    r_new--;
  }
  if(c_index >= 0)
  {
    c_new--;
  }

  if(r_index < 0)
  {
    r_index = _rows + 1;
  }

  if(c_index < 0)
  {
    c_index = _cols + 1;
  }

  Matrix m(r_new, c_new);

  int r_offset = 0;
  int c_offset = 0;

  for(int i = 0; i < r_new; i++)
  {
    if(i < r_index)
    {
      r_offset = 0;
    }
    else
    {
      r_offset = 1;
    }
    for(int j = 0; j < c_new; j++)
    {
      if(j < c_index)
      {
        c_offset = 0;
      }
      else
      {
        c_offset = 1;
      }

      m(i, j) = __get(i + r_offset, j + c_offset);
    }
  }

  *this = m;
}

void Matrix::inverse()
{
  Matrix m(_rows, _cols);

  m = *this;

  double det = m.det();
  m.adjunct();

  m *= 1.0/det;

  *this = m;
}


Matrix& Matrix::operator*=(const Matrix &m)
{
  Matrix C(_rows, _cols);
  for(int r = 0; r < _rows; r++)
  {
    for(int c = 0; c < _cols; c++)
    {
      double v = 0.0;
      for(int k = 0; k < _rows; k++)
      {
        v += (__get(r,k) * m(k,c));
      }
      C(r,c) = v;
    }
  }
  *this = C;
  return *this;
}




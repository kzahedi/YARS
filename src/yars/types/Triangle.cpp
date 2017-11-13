#include "Triangle.h"

Triangle::Triangle()
{
  uvGiven = false;
}

Triangle::Triangle(
    int first, int second, int third,
    P3D first_normal, P3D second_normal, P3D third_normal,
    P3D first_pos, P3D second_pos, P3D third_pos,
    double a_s, double a_t,
    double b_s, double b_t,
    double c_s, double c_t,
    bool given)
{
  a        = first;
  b        = second;
  c        = third;
  a_normal = first_normal;
  b_normal = second_normal;
  c_normal = third_normal;
  a_u      = a_s;
  a_v      = a_t;
  b_u      = b_s;
  b_v      = b_t;
  c_u      = c_s;
  c_v      = c_t;
  p_a      = first_pos;
  p_b      = second_pos;
  p_c      = third_pos;
  uvGiven  = given;
}

Triangle& Triangle::operator=(const Triangle &t)
{
  a        = t.a;
  b        = t.b;
  c        = t.c;

  a_normal = t.a_normal;
  b_normal = t.b_normal;
  c_normal = t.c_normal;

  p_a      = t.p_a;
  p_b      = t.p_b;
  p_c      = t.p_c;

  a_u      = t.a_u;
  a_v      = t.a_v;
  b_u      = t.b_u;
  b_v      = t.b_v;
  c_u      = t.c_u;
  c_v      = t.c_v;

  uvGiven  = t.uvGiven;

  return *this;
}

Triangle::Triangle(const Triangle &t)
{
  a        = t.a;
  b        = t.b;
  c        = t.c;

  a_normal = t.a_normal;
  b_normal = t.b_normal;
  c_normal = t.c_normal;

  p_a      = t.p_a;
  p_b      = t.p_b;
  p_c      = t.p_c;

  a_u      = t.a_u;
  a_v      = t.a_v;
  b_u      = t.b_u;
  b_v      = t.b_v;
  c_u      = t.c_u;
  c_v      = t.c_v;

  uvGiven  = t.uvGiven;
}

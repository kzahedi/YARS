#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <yars/types/P3D.h>

#include <ostream>
#include <vector>

class Triangle 
{
  public:
    int a;
    int b;
    int c;

    P3D p_a;
    P3D p_b;
    P3D p_c;

    P3D a_normal;
    P3D b_normal;
    P3D c_normal;

    yReal a_u;
    yReal a_v;

    yReal b_u;
    yReal b_v;

    yReal c_u;
    yReal c_v;

    bool uvGiven;

    Triangle();

    Triangle & operator=(const Triangle &t);
    Triangle(const Triangle &t);
    Triangle(int a, int b, int c,
        P3D a_normal, P3D b_normal, P3D c_normal,
        P3D p_a, P3D p_b, P3D p_c,
        yReal a_u = 0.0, yReal a_v = 0.0,
        yReal b_u = 0.0, yReal b_v = 0.0,
        yReal c_u = 0.0, yReal c_v = 0.0,
        bool uvGiven = false);

    friend std::ostream& operator<<(std::ostream& str, const Triangle& t)
    {
      str << "(A: " << t.a << ", " << t.a_normal << ") (B: " << t.b <<  ", " << t.b_normal << ") (C: " << t.c << ", " << t.c_normal << ")";
      return str;
    };

};

typedef std::vector<Triangle> Triangles;

#endif //__TRIANGLE_H__

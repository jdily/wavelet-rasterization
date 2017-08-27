#include <math.h>
#include <iostream>
#include <vector>
#define EQN_EPS 1e-9

extern "C"
{
typedef float real_t;
typedef int index_t;
using namespace std;

static int isZero(double x) {
    return x > -EQN_EPS && x < EQN_EPS;
}

// s size -> 1
int solveLinear(float c1, float c0, float* s) {
    if (isZero(c1)) {
        return 0;
    }
    s[0] = -c0/c1;
    return 1;
}
// s size -> 2
int solveQuadric(float c2, float c1, float c0, float* s) {
    double p, q, D;
    // make sure we have a d2 equation
    if (isZero(c2))
        return solveLinear(c1, c0, s);
    // normal for: x^2 + px + q
    p = c1 / (2.0 * c2);
    q = c0 / c2;
    D = p * p - q;
    if (isZero(D)) {
        // one double root
//        s0 = s1 = -p;
        s[0] = -p;
        s[1] = -p;
        return 1;
    }
    if (D < 0.0)
        // no real root
        return 0;
    else {
        // two real roots
        double sqrt_D = sqrt(D);
//        s0 = sqrt_D - p;
//        s1 = -sqrt_D - p;
        s[0] = sqrt_D - p;
        s[1] = -sqrt_D - p;
        return 2;
    }
}

int solveCubic(float c3, float c2, float c1, float c0, float* s) {
    int i, num = 0;
    double  sub,
        A, B, C,
        sq_A, p, q,
        cb_p, D;

    if (isZero(c3))
        return solveQuadric(c2, c1, c0, s);

    // normalize the equation:x ^ 3 + Ax ^ 2 + Bx  + C = 0
    A = c2 / c3;
    B = c1 / c3;
    C = c0 / c3;

    // substitute x = y - A / 3 to eliminate the quadric term: x^3 + px + q = 0

    sq_A = A * A;
    p = 1.0/3.0 * (-1.0/3.0 * sq_A + B);
    q = 1.0/2.0 * (2.0/27.0 * A *sq_A - 1.0/3.0 * A * B + C);

    // use Cardano's formula

    cb_p = p * p * p;
    D = q * q + cb_p;

    if (isZero(D))
    {
        if (isZero(q))
        {
            // one triple solution
//            s0 = 0.0;
            s[0] = 0.0;
            num = 1;
        }
        else
        {
            // one single and one double solution
            double u = cbrt(-q);
//            s0 = 2.0 * u;
//            s1 = - u;
            s[0] = 2.0*u; s[1] = -u;
            num = 2;
        }
    }
    else
        if (D < 0.0)
        {
            // casus irreductibilis: three real solutions
            double phi = 1.0/3.0 * acos(-q / sqrt(-cb_p));
            double t = 2.0 * sqrt(-p);
//            s0 = t * cos(phi);
//            s1 = -t * cos(phi + M_PI / 3.0);
//            s2 = -t * cos(phi - M_PI / 3.0);
            s[0] = t*cos(phi);
            s[1] = -t*cos(phi+M_PI/3.0);
            s[2] = -t*cos(phi-M_PI/3.0);
            num = 3;
        }
        else
        {
            // one real solution
            double sqrt_D = sqrt(D);
            double u = cbrt(sqrt_D + fabs(q));
            if (q > 0.0)
                s[0] = -u + p / u;
//                s0 = - u + p / u ;
            else
                s[0] = u - p / u;
//                s0 = u - p / u;
            num = 1;
        }

        // resubstitute
        sub = 1.0 / 3.0 * A;
        s[0] -= sub; s[1] -= sub; s[2] -= sub;
//        s0 -= sub;
//        s1 -= sub;
//        s2 -= sub;
        return num;
}


} // extern "C"
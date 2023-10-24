#pragma once
#include <string>

class Vec
{
public:
    double x, y;

    Vec();
    Vec(double x, double y);

    Vec operator+ (const Vec& rhs); 
    Vec operator- (const Vec& rhs);
    Vec operator* (double rhs);

    Vec operator/(double rhs);

    void operator+= (const Vec& rhs); 
    void operator-= (const Vec& rhs);
    void operator*= (double rhs);
    void operator/= (double rhs);

    std::string toString();
    double length();
    double lengthSqr();
    void clampToLength(double maxL);
    void setToLength(double newL);
    void normalize();
    Vec normalized();
    double distanceTo(Vec rhs);
};


double dot(Vec a, Vec b);
double cross(Vec a, Vec b);
double interpolate(double a, double b, double t);
double clamp(double, double, double);

Vec interpolate(Vec a, Vec b, double t);
Vec projAB(Vec a, Vec b);
Vec reflection(Vec vector, Vec normal);

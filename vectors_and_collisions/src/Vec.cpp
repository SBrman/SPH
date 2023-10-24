#include <cmath>
#include "Vec.hpp"

Vec::Vec(): x(0), y(0) { }

Vec::Vec(double x, double y) { 
    this->x = x; 
    this->y = y; 
}

Vec Vec::Vec::operator+ (const Vec& rhs) {
	return Vec(x + rhs.x, y + rhs.y);
}

Vec Vec::Vec::operator- (const Vec& rhs) {
	return Vec(x - rhs.x, y - rhs.y);
}

Vec Vec::Vec::operator* (double rhs) {
	return Vec(x * rhs, y * rhs);
}

Vec Vec::Vec::operator/ (double rhs) {
	return Vec(x / rhs, y / rhs);
}

void Vec::Vec::operator+= (const Vec& rhs) {
	x += rhs.x;
	y += rhs.y;
}

void Vec::Vec::operator-= (const Vec& rhs) {
	x -= rhs.x;
	y -= rhs.y;
}

void Vec::Vec::operator*= (double rhs) {
	x *= rhs;
	y *= rhs;
 }

void Vec::Vec::operator/= (double rhs) {
	x /= rhs;
	y /= rhs;
 }


std::string Vec::toString() {
    return "Vector(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

double Vec::length() {
    return std::sqrt(x * x + y * y);
}

double Vec::lengthSqr() {
    return x * x + y * y;
}

void Vec::clampToLength(double maxL) {
    double magnitude = std::sqrt(x * x + y * y);
    if (magnitude > maxL) {
        x *= maxL / magnitude;
        y *= maxL / magnitude;
    }
}

void Vec::setToLength(double newL) {
    double magnitude = std::sqrt(x * x + y * y);
    x *= newL / magnitude;
    y *= newL / magnitude;
}

void Vec::normalize() {
    double magnitude = std::sqrt(x * x + y * y);
    x /= magnitude;
    y /= magnitude;
}

Vec Vec::normalized() {
    double magnitude = std::sqrt(x * x + y * y);
    if (magnitude <= 0 || magnitude > 1000000)
        return Vec{ 0.0, 0.0 };
    return Vec(x / magnitude, y / magnitude);
}

double Vec::distanceTo(Vec rhs) {
    double dx = rhs.x - x;
    double dy = rhs.y - y;
    return std::sqrt(dx * dx + dy * dy);
}

Vec interpolate(Vec a, Vec b, double t) {
    return a + ((b - a) * t);
}

double interpolate(double a, double b, double t) {
    return a + ((b - a) * t);
}

double dot(Vec a, Vec b) {
    return a.x * b.x + a.y * b.y;
}

double cross(Vec a, Vec b) {
    return a.x * b.y - b.x * a.y;
}

Vec projAB(Vec a, Vec b) {
    return b * (a.x * b.x + a.y * b.y);
}

Vec reflection(Vec vector, Vec normal) {
    return vector - normal.normalized() * dot(vector, normal.normalized()) * 2;
}

double clamp(double n, double lower, double upper) {
  return std::max(lower, std::min(n, upper));
}
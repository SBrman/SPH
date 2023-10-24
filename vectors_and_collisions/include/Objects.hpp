#pragma once
#include <string>
#include "Vec.hpp"
#include "SFML/Graphics.hpp"


struct Shape {
    float restitution = 0.8f;

    int id{ 0 };
    char type{ 's' };
    virtual ~Shape() {}
    virtual std::string to_string() { return "Shape"; }
};

struct Line : Shape {
    Vec p1, p2;
    
    sf::RectangleShape sfml_repr;

    Line() : p1(Vec()), p2(Vec()) {}
    Line(int, double, double, double, double);
    std::string to_string();
};

struct Circle : Shape {
	double x, y, radius;

    Circle() : x(0), y(0), radius(0) {}
    Circle(int, double, double, double);
    std::string to_string();
};

struct Box : Shape {
	double x, y, width, height;
    double x_left, x_right, y_bottom, y_top;

    Box() : x(0), y(0), width(0), height(0) {}
    Box(int, double, double, double, double);
    std::string to_string();
};

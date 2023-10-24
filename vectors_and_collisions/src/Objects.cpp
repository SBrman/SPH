#include "Objects.hpp"
#include "Vec.hpp"
#include "SFML/Graphics.hpp"

Line::Line(int id, double x1, double y1, double x2, double y2) {
    this->id = id;
    p1 = Vec(x1, y1);
    p2 = Vec(x2, y2);
    type = 'l';
    
    sfml_repr.setPosition(sf::Vector2f(p1.x, p1.y));
    sfml_repr.setSize(sf::Vector2f(p2.x, p2.y));
}

std::string Line::to_string() {
    return "Line(x1=" + std::to_string(p1.x) + ", y1=" + std::to_string(p1.y) +
        ", x2=" + std::to_string(p2.x) + ", y2=" + std::to_string(p2.y) + ")";
}


Box::Box(int id, double x, double y, double width, double height) {
    this->id = id;
    this->x = x; 
    this->y = y;
    this->width = width; 
    this->height = height;
    type = 'b';

    x_left = x - width / 2;
    x_right = x + width / 2;
    y_bottom = y - height / 2;
    y_top = y + height / 2;
}

std::string Box::to_string() {
    return "Box(x=" + std::to_string(x) + ", y=" + std::to_string(y)
        + ", width=" + std::to_string(width) + ", height=" + std::to_string(height) + ")";
}

Circle::Circle(int id, double x, double y, double radius) {
    this->id = id;
    this->x = x; 
    this->y = y;
    this->radius = radius;
    type = 'c';
}

std::string Circle::to_string() {
    return "Circle(x=" + std::to_string(x) + ", y=" + std::to_string(y)
        + ", radius=" + std::to_string(radius) + ")";
}

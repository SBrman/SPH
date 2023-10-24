#include <cmath>
#include <unordered_set>
#include "Vec.hpp"
#include "Objects.hpp"
#include <iostream>
#include <vector>
#include "..\..\pinball\BoxRigid.h"
#include "CollisionDetection.hpp"



bool same_side(Line& line, Vec& p1, Vec& p2) {
    double cp1 = cross(line.p2 - line.p1, p1 - line.p1);
    double cp2 = cross(line.p2 - line.p1, p2 - line.p1);
    return (cp1 * cp2 >= 0);
}

bool line_line_intersect(Line& a, Line& b) {
    if (same_side(a, b.p1, b.p2)) return false;
    if (same_side(b, a.p1, a.p2)) return false;
    return true;
}

// bool line_circle_intersect(Line& line, Circle& circle) {
//     Vec toCircle = Vec{circle.x, circle.y} - line.p1;
//     Vec l_vec = line.p2 - line.p1;
//     
//     double a = l_vec.lengthSqr();
//     double b = 2 * dot(l_vec, toCircle);
//     double c = toCircle.lengthSqr() - (circle.radius * circle.radius);
//     
//     double discriminant = b * b - 4 * a * c;
//     if (discriminant < 0) return false;
//     
//     double t1 = (-b - std::sqrt(discriminant)) / (2 * a);
//     double t2 = (-b + std::sqrt(discriminant)) / (2 * a);
//     
//     if (t1 >= 0 && t1 <= 1 || t2 >= 0 && t2 <= 1) { return true; }
//     
//     return false;
// }

Vec closest_point(Line& line, Circle& circle) {
    Vec lineToCircle = Vec{circle.x, circle.y} - line.p1;
    Vec lineVector = line.p2 - line.p1;
    
    Vec lineNormalized = lineVector.normalized();
    float projection = lineToCircle.x * lineNormalized.x + lineToCircle.y * lineNormalized.y;

    Vec closestPoint;
    if (projection <= 0.0f) {
        closestPoint = line.p1;
    } else if (projection >= lineVector.length()) {
        closestPoint = line.p2;
    } else {
        closestPoint = line.p1 + lineNormalized * projection;
    }
    
    return closestPoint;
}

bool line_circle_intersect(Line& line, Circle& circle) {
    return Vec{circle.x, circle.y}.distanceTo(closest_point(line, circle)) <= circle.radius;
}


bool point_in_box(Vec& p, double lx, double by, double rx, double ty) {
    if (p.x >= lx && p.x <= rx && p.y >= by && p.y <= ty) 
        return true;
    return false;
}

bool line_box_intersect(Line& a, Box& b) {

    double lx = b.x - b.width/2;
    double by = b.y - b.height/2;
    double rx = b.x + b.width/2;
    double ty = b.y + b.height/2;

    Line top {0, lx, ty, rx, ty};
    Line bottom {0, lx, by, rx, by};
    Line left {0, lx, by, lx, ty};
    Line right {0, rx, by, rx, ty};

    Line lines[4] = {top, bottom, left, right};
    for (auto& line : lines)
        if (line_line_intersect(a, line)) 
            return true;
    
    if (point_in_box(a.p1, lx, by, rx, ty) || point_in_box(a.p2, lx, by, rx, ty)) 
        return true;
    
    return false;
}

bool circle_circle_intersect(Circle& a, Circle& b) {
    Vec ab {a.x - b.x, a.y - b.y};
    return (a.radius + b.radius >= ab.length());
}

Vec closest_point(Circle& c, Box& b) {
    return Vec {
        clamp(c.x, b.x - b.width/2, b.x + b.width/2), 
        clamp(c.y, b.y - b.height/2, b.y + b.height/2)
    };
}

bool circle_box_intersect(Circle& c, Box& b) {
    if (closest_point(c, b).distanceTo(Vec{c.x, c.y}) <= c.radius)
        return true;
    return false;
}

bool circle_box_intersect(Circle& c, BoxRigid& b) {
    for (int i = 0; i < 4; ++i) {
        int j = (i < 3) ? i + 1 : 0;
        Line line {0, b.points[i].x, b.points[i].y, b.points[j].x, b.points[j].y};
        if (line_circle_intersect(line, c))
            return true;
    }

    return false;
}


bool point_in_box(Vec point, BoxRigid& box) {
    Vec relative_pos = point - box.center;

    Vec box_right = Vec(std::cos(box.angle), std::sin(box.angle));
    Vec box_up = Vec(std::sin(box.angle), std::cos(box.angle));

    float point_right = dot(relative_pos, box_right);
    float point_up = dot(relative_pos, box_up);

    if ((std::abs(point_right) < box.w/2) && (std::abs(point_up) < box.h/2))
        return true;

    return false;
}


bool box_box_intersect(Box& a, Box& b) {

    if (a.x_right < b.x_left) return false;
    if (a.y_top < b.y_bottom) return false;
    if (b.x_right < a.x_left) return false;
    if (b.y_top < a.y_bottom) return false;

    return true;
}











void collisions_in_scene(const std::vector<Shape*>& shapes, std::unordered_set<int>& collision_ids) {

    bool collision = false;

    // Check for collisions
    for (int i = 0; i < shapes.size(); ++i) {
        for (int j = i; j < shapes.size(); ++j) {
            Shape shape = *(shapes)[i];
            Shape shape2 = *(shapes)[j];

            if (shape.id == shape2.id) continue;
            switch (shape.type) {
            case 'l':
                switch (shape2.type) {
                case 'l': collision = line_line_intersect(*static_cast<Line*>((shapes)[i]), *static_cast<Line*>(shapes[j])); break;
                case 'b': collision = line_box_intersect(*static_cast<Line*>(shapes[i]), *static_cast<Box*>(shapes[j])); break;
                case 'c': collision = line_circle_intersect(*static_cast<Line*>(shapes[i]), *static_cast<Circle*>(shapes[j])); break;
                } break;
            case 'b':
                switch (shape2.type) {
                    case 'l': collision = line_box_intersect(*static_cast<Line*>(shapes[j]), *static_cast<Box*>(shapes[i])); break;
                    case 'b': collision = box_box_intersect(*static_cast<Box*>(shapes[i]), *static_cast<Box*>(shapes[j])); break;
                    case 'c': collision = circle_box_intersect(*static_cast<Circle*>(shapes[j]), *static_cast<Box*>(shapes[i])); break;
                } break;
            case 'c':
                switch (shape2.type) {
                case 'l': collision = line_circle_intersect(*static_cast<Line*>(shapes[j]), *static_cast<Circle*>(shapes[i])); break;
                case 'b': collision = circle_box_intersect(*static_cast<Circle*>(shapes[i]), *static_cast<Box*>(shapes[j])); break;
                case 'c': collision = circle_circle_intersect(*static_cast<Circle*>(shapes[i]), *static_cast<Circle*>(shapes[j])); break;
                } break;
            }

            if (collision) {
                collision_ids.insert(shape.id);
                collision_ids.insert(shape2.id);
                collision = false;
            }
        }
    }
}

void collisions_in_scene(const std::vector<Line>& lines, const std::vector<Circle>& circles, const std::vector<Box>& boxes, std::unordered_set<int>& collision_ids) {
    
	// Check for collisions
	for (Line line : lines) {

		for (Line line2 : lines) {
            if (line.id == line2.id) continue;
			if (line_line_intersect(line, line2)) {
				collision_ids.insert(line.id); collision_ids.insert(line2.id);
			}
		}
		for (Box box : boxes) {
            if (line_box_intersect(line, box)) {
				collision_ids.insert(line.id); collision_ids.insert(box.id);
			}
		}
		for (Circle circle : circles) {
            if (line_circle_intersect(line, circle)) {
				collision_ids.insert(line.id); collision_ids.insert(circle.id);
			}
		}
	}

	for (Box box : boxes) {
		for (Box box2 : boxes) {
            if (box.id == box2.id) continue;
            if (box_box_intersect(box, box2)) {
				collision_ids.insert(box.id); collision_ids.insert(box2.id);
			}
		}
		for (Circle circle : circles) {
            if (circle_box_intersect(circle, box)) {
				collision_ids.insert(box.id); collision_ids.insert(circle.id);
			}
		}
	}

	for (Circle circle : circles) {
		for (Circle circle2 : circles) {
            if(circle.id == circle2.id) continue;
            if (circle_circle_intersect(circle, circle2)) {
				collision_ids.insert(circle.id); collision_ids.insert(circle2.id);
			}
		}
	}


 }
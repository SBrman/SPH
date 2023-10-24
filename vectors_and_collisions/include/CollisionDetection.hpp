#pragma once
#include "Objects.hpp"
#include "Vec.hpp"
#include <unordered_set>
#include <vector>
#include "BoxRigid.h"

bool same_side(Line& line, Vec& p1, Vec& p2);
bool line_line_intersect(Line& a, Line& b);
bool line_circle_intersect(Line& a, Circle& b);
bool line_box_intersect(Line& a, Box& b);
bool circle_circle_intersect(Circle& a, Circle& b);
bool circle_box_intersect(Circle& c, Box& b);

bool circle_box_intersect(Circle& c, BoxRigid& b);

bool box_box_intersect(Box& a, Box& b);

Vec closest_point(Line& line, Circle& circle);
Vec closest_point(Circle& c, Box& b);


void collisions_in_scene(const std::vector<Shape*>& shapes, std::unordered_set<int>& collision_ids);

void collisions_in_scene(const std::vector<Line>& lines, const std::vector<Circle>& circles, const std::vector<Box>& boxes, std::unordered_set<int>& collision_ids);

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include "Vec.hpp"
#include "Objects.hpp"


int get_objects(std::string filename, std::vector<Line>& lines, std::vector<Box>& boxes, std::vector<Circle>& circles) {

	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl; return 1;
	}

	// To keep track of the last object type read
	enum class Object { line, box, circle };
	Object active_object = Object::line;
	// To keep track of the number of maximum objects of each type
	int line_max = 0, box_max = 0, circle_max = 0;


	std::string current_line;
	while (std::getline(file, current_line)) {
		if (current_line.empty() || current_line[0] == '#') continue;

		std::regex pattern(R"((\w*):\s+(\d+))");
		std::smatch matches;
		if (std::regex_match(current_line, matches, pattern)) {
			if (matches[1] == "Lines") {
				active_object = Object::line;
				line_max = std::stoi(matches[2]);
			}
			else if (matches[1] == "Boxes") {
				active_object = Object::box;
				box_max = std::stoi(matches[2]);
			}
			else if (matches[1] == "Circles") {
				active_object = Object::circle;
				circle_max = std::stoi(matches[2]);
			}
			else {
				std::cerr << "unknown object type: " << matches[1] << std::endl;
			}
			continue;
		}

		if (active_object == Object::line) {
			std::regex pattern(R"((\d+)\s:\s(\d.\d+)\s(\d.\d+)\s(\d.\d+)\s(\d.\d+))");
			std::smatch matches;
			if (std::regex_match(current_line, matches, pattern)) {
				int id = std::stoi(matches[1]);
				double x1 = std::stof(matches[2]);
				double y1 = std::stof(matches[3]);
				double x2 = std::stof(matches[4]);
				double y2 = std::stof(matches[5]);
				lines.push_back(Line(id, x1, y1, x2, y2));
			}
		}
		else if (active_object == Object::box) {
			std::regex pattern(R"((\d+)\s:\s(\d.\d+)\s(\d.\d+)\s(\d.\d+)\s(\d.\d+))");
			std::smatch matches;
			if (std::regex_match(current_line, matches, pattern)) {
				int id = std::stoi(matches[1]);
				double x = std::stof(matches[2]);
				double y = std::stof(matches[3]);
				double width = std::stof(matches[4]);
				double height = std::stof(matches[5]);
				boxes.push_back(Box(id, x, y, width, height));
			}
		}
		else if (active_object == Object::circle) {
			std::regex pattern(R"((\d+)\s:\s(\d.\d+)\s(\d.\d+)\s(\d.\d+))");
			std::smatch matches;
			if (std::regex_match(current_line, matches, pattern)) {
				int id = std::stoi(matches[1]);
				double x = std::stof(matches[2]);
				double y = std::stof(matches[3]);
				double radius = std::stof(matches[4]);
				circles.push_back(Circle(id, x, y, radius));
			}
		}
	}

	int return_code = 1;
	// Checks if the maximum number of objects of each type has been reached
	if (lines.size() == line_max && boxes.size() == box_max && circles.size() == circle_max) {
		std::cout << "All objects read" << std::endl;
		return_code = 0;
	}
	else {
		std::cerr << "Not all objects read" << std::endl;
	}

	file.close();
	return return_code;
}
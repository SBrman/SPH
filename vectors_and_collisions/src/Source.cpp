#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <ratio>
#include <memory>
#include <algorithm>

#include "Vec.h"
#include "Objects.h"
#include "CollisionDetection.h"

#include "Reader.h"
#include "Source.h"


int write_things(std::string filePath, double duration, std::unordered_set<int> collision_ids) {

	std::ofstream file(filePath);
	if (!file.is_open()) {
		std::cout << "Could not open file " << filePath << std::endl; 
		return 1;
	}

	file << "Duration: " << duration << " ms" << std::endl;
	file << "Num Collisions: " << collision_ids.size() << std::endl;

	std::vector<int> cids{ collision_ids.begin(), collision_ids.end() };
	std::sort(cids.begin(), cids.end());
	for (int id : cids) {
		file << id << std::endl;
	}

	file.close();
	if (file.fail()) return 1;

	return 0;
}


int get_collisions(std::string filename) {

	// To store the objects
	std::vector<Circle> circles;
	std::vector<Box> boxes;
	std::vector<Line> lines;

	get_objects(filename, lines, boxes, circles);

	std::vector <Shape*> shapes;
	for (auto line : lines) { shapes.push_back(&line); }
	for (auto box : boxes) { shapes.push_back(&box); }
	for (auto circle : circles) { shapes.push_back(&circle); }

	std::cout << "#Lines = " << lines.size() << ", #Boxes = " << boxes.size()
		<< ", #Circles = " << circles.size() << std::endl;

	std::unordered_set<int> collision_ids;

	auto t1 = std::chrono::high_resolution_clock::now();

	// collisions_in_scene(shapes, collision_ids);
	collisions_in_scene(lines, circles, boxes, collision_ids);
	auto t2 = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double, std::nano> ns = t2 - t1;
	double dur = ns.count() / 1000000.0;

	std::cout << "Time: " << ns.count() << " nanoseconds" << std::endl;
	std::cout << "Total Objects: " << shapes.size() << std::endl;
	std::cout << "Total collisions: " << collision_ids.size() << std::endl;
	
	std::string solutionPath = "solutions/" + filename.substr(0, filename.length() - 4) + "_solution.txt";
	write_things(solutionPath, dur, collision_ids);

	return 0;
}

int main() {

	//tests();

	//Read the objects from the file and store them in the vectors
	std::string filename = "CollisionTasks/task";
	for (int i = 1; i <= 10; ++i) {
		std::cout << "Task " << i << std::endl;
		get_collisions(filename + std::to_string(i) + ".txt");
		std::cout << std::endl;
	}

	return 0;
}

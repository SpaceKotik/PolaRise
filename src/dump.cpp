#include <iostream>
#include "dump.hpp"

Dump::Dump() {
	output.clear();
}

void Dump::add(std::string str) {
	output.push_back(str);
}

void Dump::display() {
	//std::cout << "\033[2J\033[1;1H";
	std::cout << "###############################\n";
	for (auto e : output) {
		std::cout << e << "\n";
	}
	std::cout << "###############################\n";
	output.clear();

}
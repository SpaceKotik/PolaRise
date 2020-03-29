#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;


class Dump {
private:
	std::vector<std::string> output;
public:
	Dump();
	void add(std::string str);
	void display();
};
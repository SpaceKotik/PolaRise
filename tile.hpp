#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Tile {
private:
	RectangleShape physForm;
public:
	Tile();
	void rotate(float deg);

friend class Game;
};
#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Tile {
private:
	RectangleShape physForm;
public:
	Tile();
	void rotate(float deg);
	RectangleShape getRect();

friend class Game;
friend class Level;
};
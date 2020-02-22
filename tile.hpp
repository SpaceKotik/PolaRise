#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Tile {
private:
	RectangleShape physForm;
	bool isRed;
	bool isBlue;
public:
	Tile();
	Tile(Vector2f pos, bool isRed, bool isBlue);
	void rotate(float deg);
	RectangleShape getRect();
	bool checkIfBlue();

friend class Game;
friend class Level;
};
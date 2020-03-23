#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

enum TileType {Standart, StartPos, FinishPos, Lava};

class Tile {
private:
	RectangleShape physForm;
	TileType typeRed = Standart;
	TileType typeBlue = Standart;
	bool isRed;
	bool isBlue;
	bool isSolidRed = false;
	bool isSolidBlue = false;

	bool isDynamic = true;
public:
	Tile();
	Tile(Vector2f pos, bool isRed, bool isBlue);
	void rotate(float deg);
	RectangleShape getRect();
	bool checkIfBlue();
	bool checkIfRed();
	bool checkIfSolidRed();
	bool checkIfSolidBlue();

friend class Game;
friend class Level;
};
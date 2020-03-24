#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

enum TileType {Standart, StartPos, FinishPos, Lava};

class Tile {
private:
	RectangleShape physForm;

	bool isRed;
	bool isBlue;
	bool isSolidRed = false;
	bool isSolidBlue = false;

    TileType type = Standart;
    bool isSolid = false;
	bool isDynamic = true;
	bool isActive = false;
public:
	Tile();
	Tile(Vector2f pos, bool isRed, bool isBlue);
	RectangleShape getRect();
	bool checkIfBlue();
	bool checkIfRed();
	bool checkIfSolidRed();
	bool checkIfSolidBlue();

friend class Game;
friend class Level;
};
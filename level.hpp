#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"

using namespace sf;

struct Field {
	Tile *tile;
	bool isRed;
	bool isBlue;
};

class Level {
private:
	Field field;
//	Enemy *enemy;
	Vector2i fieldSize; //in tile coords
	Vector2f startPos;
	Vector2f finishPos;
	int tileCount;


public:
	Level();
	void setField();
	Field* getField();
	int getTileCount();
	void update();


};
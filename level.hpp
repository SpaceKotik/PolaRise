#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"

using namespace sf;

struct Field {
	std::vector<Tile> tiles;
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
	void addTile(Vector2f);
	void removeTile(Vector2f);
};
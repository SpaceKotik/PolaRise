#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

#include "tile.hpp"
#include "util/eMath/eMath.h"
#include "consts.h"

using namespace sf;
class Game;

enum LevelState {Red, Blue};

struct Field {
	std::vector<Tile> tiles;
};



class Level {
private:
	Field field;
	Vector2f startPos;
	Vector2f finishPos;
	Game* mediator;
public:
	Level();
	void setField();
	Field* getField();
	void update();
	void addTile(Vector2f, TileType type);
	void removeTile(Vector2f);
	int loadToFile();
	int loadFromFile();
	bool isOnTile(Vector2f pos);
	bool isOnFinish(Vector2f pos);

    void resetActive();                                 /// set all tiles as unlighted (inactive)
	void setActiveTile(Vector2f pos);                   /// set tile where pos lies as active
	void setDynamicTiles(std::array<Vertex, 2> line);   /// set all tiles intersecting *line* as active

	void setMediator(Game* game);
};
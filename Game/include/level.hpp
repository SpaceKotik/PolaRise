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


class Level {
private:
    std::vector<Tile> level;
	Vector2f startPos = {0, 0};
	Vector2f finishPos = {0, 0};
	Game* mediator;
public:
	Level();
	void setField();
    std::vector<Tile>* getField();
	void update();
	void addTile(Vector2f, TileType type);
	void removeTile(Vector2f);
	int loadToFile();
	int loadFromFile();
	bool isOnTile(Vector2f pos);
	bool isOnFinish(Vector2f pos);
	bool isOnDeadly(Vector2f pos);
	bool isUnderPlayer(Vector2f pos);

    void resetActive();                                 /// set all tiles as unlighted (inactive)
	void setActiveTile(Vector2f pos);                   /// set tile where pos lies as active
	void setDynamicTiles(std::array<Vertex, 2> line);   /// set all tiles intersecting *line* as active

	void setMediator(Game* game);
    eVector2f getStartPos();
};
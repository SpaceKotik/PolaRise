#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"

using namespace sf;

enum LevelState {Red, Blue};

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
	LevelState levelState = Blue;
public:
	Level();
	void setField();
	Field* getField();
	void update();
	void addTile(Vector2f);
	void removeTile(Vector2f);
	int loadToFile();
	int loadFromFile();
	void changeState();
	LevelState getState();
	bool isOnTile(Vector2f pos);
	bool isOnFinish(Vector2f pos);

    void resetActive();


	void setActiveTile(Vector2f pos);
	void setDynamicTiles(std::array<Vertex, 2> Line);
};
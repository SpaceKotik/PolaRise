#pragma once

#include <SFML/Graphics.hpp>
#include "consts.h"

using namespace sf;

enum TileType{
    Standart,
    Dynamic,
    Void,
    Deadly,
    Start,
    Finish
};

class Tile {
private:
	RectangleShape physForm;
    TileType type = Standart;
	bool isActive = false;
	bool underPlayer = false;
public:
	Tile();
	Tile(Vector2f pos, TileType _type);
	RectangleShape getRect();

	bool isSolid();
	bool isLightAbsorb();
	bool isDrawable();
	bool isDeadly();
	bool isFinish();
	bool isUnderPlayer();

friend class Game;
friend class Level;
friend class RayTracing;
friend class Player;
};
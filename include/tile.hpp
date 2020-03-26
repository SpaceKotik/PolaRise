#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

enum TileType {Standart, Dynamic, Void, StartPos, FinishPos};

class Tile {
private:
	RectangleShape physForm;
    TileType type = Standart;
	bool isActive = false;
public:
	Tile();
	Tile(Vector2f pos, TileType _type);
	RectangleShape getRect();

	bool checkIfSolid();
	bool checkIfLightAbsorb();

friend class Game;
friend class Level;
friend class RayTracing;
friend class Player;
};
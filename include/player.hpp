#pragma once

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "level.hpp"
enum TypeInput {Pressed, Released};
using namespace sf;



struct PlayerStates {
    bool right = 0;
    bool left = 0;
    bool space = 0;
    bool jumpAble = true;
    bool jumpMade = false;
    Clock jumpTime;

    bool isOnStart = 0;
    bool isOnFinish = 0;
};



class PlayerState;

class Player {
private:
	RectangleShape physForm;/// Ddfines player's position, collision rectangle, color etc
    Vector2f view;          /// vector defining where player looks to
	Vector2f velocity = Vector2f(0, 0);

    PlayerStates states;    /// states used for processing input
    Level* level;           /// pointer to the current level (for processing collisions etc)
public:
	Player();
	void move(Vector2f dir);        /// moves player by dir vector
	void setPos(Vector2f pos);      /// sets player's position
	Vector2f getPos();              /// returns player's position
	RectangleShape* getPhysForm();  /// returns pointer to physForm for drawing
    void setLevel(Level* _level);   /// sets pointer to the current level
    void updateMovement();          /// processes player movement based on input
    void disableDynamicTiles();     ///set dynamic  tiles which player intersect inactive

	friend class Game;

};
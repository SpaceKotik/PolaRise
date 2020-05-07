#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include "level.hpp"
#include "consts.h"
#include "lantern.h"

using namespace sf;
class Game;

struct PlayerStates {
    bool right = 0;
    bool left = 0;
    bool space = 0;
    bool jumpAble = true;
    bool jumpMade = false;
    Clock jumpTime;
    bool ShootAble;
    Clock sinceLastShot;

    bool isOnStart = 0;
    bool isOnFinish = 0;
};


class Player {
private:
	RectangleShape physForm;/// Dfines player's position, collision rectangle, color etc
    eVector2f view;          /// vector defining where player looks to
	Vector2f velocity = Vector2f(0, 0);
    PlayerStates states;    /// states used for processing input
    Lantern flashLight;

    Game* mediator;
public:
	Player();
	void update(Vector2f mousePos);
	void move(Vector2f dir);        /// moves player by dir vector
	void setPos(Vector2f pos);      /// sets player's position
	Vector2f getPos();              /// returns player's position
	eVector2f getView();
	RectangleShape* getPhysForm();  /// returns pointer to physForm for drawing
    void updateMovement();          /// processes player movement based on input
    void disableDynamicTiles();     ///set dynamic  tiles which player intersect inactive
    void reset();

    void setMediator(Game* game);

	friend class Game;

};
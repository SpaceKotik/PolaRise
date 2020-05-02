#include <cmath>
#include "player.hpp"
#include "consts.h"
#include "game.hpp"
using namespace consts;

Player::Player() {
	physForm.setSize({heroWidth, heroHeight});
	physForm.setOrigin(Vector2f(heroWidth/2, heroHeight/2));
	physForm.setPosition(Vector2f(100, 100));
	physForm.setFillColor(heroColor);
	physForm.setOutlineColor(Color(20, 20, 20));
	physForm.setOutlineThickness(0);
}

void Player::move(Vector2f dir) {
	physForm.move(dir);
}

void Player::setPos(Vector2f pos) {
	physForm.setPosition(pos);
}

Vector2f Player::getPos() {
	return physForm.getPosition();
}

eVector2f Player::getView() {
    return view.norm();
}

RectangleShape* Player::getPhysForm() {
	return &physForm;
}

void Player::disableDynamicTiles() {

    FloatRect playerRect = physForm.getGlobalBounds();
    ///extending rectangle to apply library logic to detect intersections on rectangle's right and bottom sides
    playerRect.top += playerOffset;
    playerRect.left += playerOffset;
    playerRect.width -= 2*playerOffset;
    playerRect.height -= 2*playerOffset;
    ///check all tiles and set inactive all intersecting
    for (auto &e: *(mediator->getLevel()->getField())) {
        e.isUnderPlayer = e.getRect().getGlobalBounds().intersects(playerRect);
    }
}

void Player::updateMovement() {
    //flashLight.sprite.setPosition(physForm.getPosition());




    if (states.jumpMade) {
        velocity.y -= maxVelocity;
        states.jumpTime.restart();
        states.space = true;
        states.jumpMade = false;
    }

    // TODO: fix player jumping in air
    if (states.jumpTime.getElapsedTime().asSeconds() > 0.24) {
        states.space = false;
        states.jumpAble = false;
        states.jumpTime.restart();
    }

    ///process input
    if (states.right)
        velocity.x += heroAcceleration;
    if (states.left)
        velocity.x -= heroAcceleration;
    if (states.space) {
        velocity.y += jumpGravity;
    }
    else {
        velocity.y += fallGravity;
    }

    ///set position that player tries to move to
    Vector2f desPos;
    ///first move by x, then apply restrictions
    move(Vector2f(velocity.x, 0));
    ///process right moving if player moves right
    desPos = getPos() + Vector2f(velocity.x, 0);
    ///if no intersections, move only horizontally
    if (mediator->getLevel()->isOnTile(desPos + Vector2f(heroWidth/2 - .1, heroHeight/2 - .1)) ||
            mediator->getLevel()->isOnTile(desPos + Vector2f(heroWidth/2 - .1, -heroHeight/2 + .1)) ||
            mediator->getLevel()->isOnTile(desPos + Vector2f(heroWidth/2 - .1, 0))) {

        ///else move right to the obstacle and mirror speed
        setPos(Vector2f(((int) getPos().x / (int) scale + 1) * scale - heroWidth/2, getPos().y));
        velocity.x = 0;
    }


    ///process left moving if player moves left
    desPos = getPos() + Vector2f(velocity.x, 0);

    if (mediator->getLevel()->isOnTile(desPos + Vector2f(-heroWidth/2 + .1, heroHeight/2 - .1)) ||
            mediator->getLevel()->isOnTile(desPos + Vector2f(-heroWidth/2 + .1, -heroHeight/2 + .1)) ||
            mediator->getLevel()->isOnTile(desPos + Vector2f(-heroWidth/2 + .1, 0))) {

        setPos(Vector2f(((int) getPos().x / (int) scale) * scale + heroWidth/2, getPos().y));
        velocity.x = 0;
    }

    ///first move by y, then apply restrictions
    move(Vector2f(0, velocity.y));
    ///process up moving if player moves up
    desPos = getPos() + Vector2f(0, velocity.y);

    if (mediator->getLevel()->isOnTile(desPos + Vector2f(heroWidth/2 - .1, -heroHeight/2 + .1)) ||
            mediator->getLevel()->isOnTile(desPos + Vector2f(-heroWidth/2 + .1, -heroHeight/2 + .1))) {

        states.space = false;
        states.jumpAble = false;

        setPos(Vector2f(getPos().x, ((int)getPos().y / (int) scale) * scale + heroHeight/2));
        velocity.y = -velocity.y * 0.2;
    }

    ///process down moving if player moves down
    desPos = getPos() + Vector2f(0, velocity.y);

    if (mediator->getLevel()->isOnTile(desPos + Vector2f(heroWidth/2 - .1, heroHeight/2 - .1)) ||
            mediator->getLevel()->isOnTile(desPos + Vector2f(-heroWidth/2 + .1, heroHeight/2 - .1)) ) {

        setPos(Vector2f(getPos().x, ((int) getPos().y / (int) scale + 1) * scale - heroHeight/2));
        velocity.y = 0;
    }
    else {
        states.jumpAble = false;
    }
    if (velocity.y == 0)
        states.jumpAble = true;///////////////



    ///decrease velocity if no input
    if (!states.right && !states.left)
        velocity = Vector2f(velocity.x * 0.83, velocity.y);



    ///restrict speed by x and y
    if(abs(velocity.x) > maxVelocity) {
        velocity.x /= abs(velocity.x);
        velocity.x *= maxVelocity;
    }

    if(abs(velocity.y) > 1.5*maxVelocity) {
        velocity.y /= abs(velocity.y);
        velocity.y *= 1.5*maxVelocity;
    }
    ///set x speed to 0 if it is too small
    if(fabs(velocity.x) <= 0.01f) {
        velocity.x = 0;
    }
}

void Player::reset() {
    physForm.setPosition(Vector2f(100, 100));
    velocity = {0, 0};
}

void Player::setMediator(Game* _game) {
    mediator = _game;
}

void Player::update(Vector2f mousePos) {
    ///normalized view
    view = eVector2f(mousePos - getPos()).norm();

    updateMovement();
    disableDynamicTiles();

    flashLight.sprite.setPosition(physForm.getPosition());
    flashLight.sprite.setRotation(-atan2(view.x, view.y)*180/M_PI + 135);

}


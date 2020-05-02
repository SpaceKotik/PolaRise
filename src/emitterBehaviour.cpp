#include "SFML/Graphics.hpp"
#include "util/eMath/eMath.h"
#include "emitterBehaviour.h"
#include "lightEmitter.h"
#include "player.hpp"
#include <iostream>
#include "consts.h"

using namespace EmitterBehaviour;
using namespace consts;
Behaviour::Behaviour() = default;
Behaviour::~Behaviour() = default;


///MoveByPath
MoveByPath::MoveByPath() {
    dir = (pos2 - pos1);
    dir = dir.norm();
}

MoveByPath::MoveByPath(const eVector2f _pos1, const eVector2f _pos2, float _speed = 1) {
    speed = _speed;
    pos1 = _pos1;
    pos2 = _pos2;
    dir = (pos2 - pos1);
    dir = dir.norm();
}

void MoveByPath::update(Emitter* emitter) {

    emitter->move(dir*speed);

    eVector2f currPos = emitter->getPosition();
    bool switched = false;
    if(currPos.x > std::max(pos1.x, pos2.x)) {
        currPos.x = std::max(pos1.x, pos2.x);
        currPos.y = (areEqual(currPos.x, pos1.x)) ? pos1.y : pos2.y;
        emitter->setPosition(currPos);
        switched = true;
    }
    else if(currPos.x < std::min(pos1.x, pos2.x)) {
        currPos.x = std::min(pos1.x, pos2.x);
        currPos.y = (areEqual(currPos.x, pos1.x)) ? pos1.y : pos2.y;
        emitter->setPosition(currPos);
        switched = true;
    }
    if(currPos.y > std::max(pos1.y, pos2.y)) {
        currPos.y = std::max(pos1.y, pos2.y);
        currPos.x = (areEqual(currPos.y, pos1.y)) ? pos1.x : pos2.x;
        emitter->setPosition(currPos);
        switched = true;
    }
    else if(currPos.y < std::min(pos1.y, pos2.y)) {
        currPos.y = std::min(pos1.y, pos2.y);
        currPos.x = (areEqual(currPos.y, pos1.y)) ? pos1.x : pos2.x;
        emitter->setPosition(currPos);
        switched = true;
    }

    if(switched) {
        if (areEqual(currPos.x, pos1.x) && areEqual(currPos.y, pos1.y)) {
            dir = pos2 - pos1;
            dir = dir.norm();
        }
        else {
            dir = pos1 - pos2;
            dir = dir.norm();
        }

    }

}


///Rotate
Rotate::Rotate(float _speed) {
    speed = _speed;
}

void Rotate::update(Emitter* emitter) {
    emitter->rotate(speed);
}


///MoveByCircle
MoveByCircle::MoveByCircle(eVector2f _center, float _radius, float _speed) {
    center = _center;
    radius = _radius;
    speed = _speed;
    angle = 0;
}

void MoveByCircle::update(Emitter* emitter) {

    angle += speed;
    while (angle >= 360)
        angle -= 360;

    Transform rotate;
    rotate.rotate(angle);
    eVector2f position(radius, 0);
    position = rotate.transformPoint(position);

    emitter->setPosition(center + position);
    position = position.norm();
    emitter->setView(-position);

}


///Flicker
Flicker::Flicker(float _speed) {
    speed = _speed;
    timer.restart();
    isActive = true;
}

void Flicker::update(Emitter* emitter) {
    emitter->setRestricted(false);
    if(timer.getElapsedTime().asSeconds() >= speed)  {
        timer.restart();
        isActive = !isActive;
        emitter->setActive(isActive);
    }
}


///BindToPlayer
BindToPlayer::BindToPlayer(Player *_player) {
    player = _player;
}

void BindToPlayer::update(Emitter* emitter) {
    emitter->setPosition(player->getPos());
    emitter->setView(player->getView());
}

///Shootable
Shootable::Shootable(Player *_player, Level *_level) {
    player = _player;
    level = _level;
    timer.restart();
    timeUntilDisappears = shootableDuration;
    speed = player->getView() * 25;
    position = player->getPos();
}

void Shootable::update(Emitter *emitter) {
    emitter->setRestricted(false);
    timeUntilDisappears -= timer.restart().asSeconds();
    position += speed;
    if (level->isOnTile(position))
        position -= speed;
    emitter->setPosition(position);
}

bool Shootable::mustBeDeleted() {
    return timeUntilDisappears <= 0;
}
#include "SFML/Graphics.hpp"
#include "emitterBehaviour.h"
#include "lightEmitter.h"

using namespace EmitterBehaviour;
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

// FIXME: if emitter's start pos is not on the first point of line, it works incorrectly
void MoveByPath::update(Emitter* emitter) {

    emitter->move(dir*speed);

    eVector2f currPos = emitter->getPosition();
    bool switched = false;
    if(currPos.x > std::max(pos1.x, pos2.x)) {
        currPos.x = std::max(pos1.x, pos2.x);
        currPos.y = (currPos.x == pos1.x) ? pos1.y : pos2.y;
        emitter->setPosition(currPos);
        switched = true;
    }
    else if(currPos.x < std::min(pos1.x, pos2.x)) {
        currPos.x = std::min(pos1.x, pos2.x);
        currPos.y = (currPos.x == pos1.x) ? pos1.y : pos2.y;
        emitter->setPosition(currPos);
        switched = true;
    }
    if(currPos.y > std::max(pos1.y, pos2.y)) {
        currPos.y = std::max(pos1.y, pos2.y);
        currPos.x = (currPos.y == pos1.y) ? pos1.x : pos2.x;
        emitter->setPosition(currPos);
        switched = true;
    }
    else if(currPos.y < std::min(pos1.y, pos2.y)) {
        currPos.y = std::min(pos1.y, pos2.y);
        currPos.x = (currPos.y == pos1.y) ? pos1.x : pos2.x;
        emitter->setPosition(currPos);
        switched = true;
    }

    if(switched) {
        if (currPos == pos1)
            dir = pos2 - pos1;
        else
            dir = pos1 - pos2;

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
    ///reset angle

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
    if(timer.getElapsedTime().asSeconds() >= speed)  {
        timer.restart();
        isActive = !isActive;
        emitter->setActive(isActive);
    }
}
#include <SFML/Graphics.hpp>

#include "rayTracing.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "lightEmitter.h"
#include <algorithm>

#include "consts.h"
#include <iostream>
using namespace consts;

#define DEFAULTEMITTER_POS eVector2f(-100, -100)
#define DEFAULTEMITTER_VIEW eVector2f(-100, -100)
#define DEFAULTEMITTER_LINEOFSIGHT 10

Emitter::Emitter() {
    position = DEFAULTEMITTER_POS;
    view = DEFAULTEMITTER_VIEW;
    isRestricted = true;
    lineOfSight = DEFAULTEMITTER_LINEOFSIGHT;
    color = lightColorRed;
    behaviour = nullptr;
}

Emitter::Emitter(const eVector2f _position, const eVector2f _view, bool _updateOnDemand, Color _color, bool _isRestricted) {
    position = _position;
    view = _view;
    isRestricted = _isRestricted;
    lineOfSight = viewAngle;
    updateOnDemand = _updateOnDemand;
    color = _color;
    behaviour = nullptr;
}

Emitter::~Emitter() {
    //std::cout << "Emitter: " << behaviour << std::endl;
    //if (behaviour != nullptr)
     //   delete behaviour;

}

void Emitter::updateLightMap(const RayTracing* _rayTracing) {
    rayTracing = *_rayTracing;
}

void Emitter::setPosition(eVector2f pos) {
    position = pos;
}

Vector2f Emitter::getPosition() const {
    return position;
}

void Emitter::setView(eVector2f _view) {
    view = _view;
}

void Emitter::setColor(Color _color) {
    color = _color;
}

void Emitter::setRestricted(bool _isRestricted) {
    isRestricted = _isRestricted;
}

bool Emitter::setLineOfSight(float _viewAngle) {
    if (viewAngle < 0)
        return false;
    lineOfSight = _viewAngle;
    return true;
}

void Emitter::setBehaviour(EmitterBehaviour::Behaviour* _behaviour) {

    if (_behaviour != nullptr) {
        delete behaviour;
        behaviour = _behaviour;
    }


}

void Emitter::rotate(const float angle) {
    view.rotate(angle);
}

void Emitter::move(const eVector2f dir) {
    position += dir;
}

///calculcates new VertexArray only if is needed
void Emitter::updateRayTracing(bool update) {
    if (!updateOnDemand || update)
        rayTracing.update(position, isRestricted, view, lineOfSight);

}

VertexArray Emitter::createMesh() {
    return rayTracing.createMesh(color);
}

void Emitter::setActiveTiles(Level *level) {
    rayTracing.setActiveTiles(level);
}

void Emitter::update() {
    if (behaviour != nullptr)
        behaviour->update(this);
}






using namespace EmitterBehaviour;
Behaviour::Behaviour() = default;

Behaviour::~Behaviour() = default;

MoveByPath::MoveByPath() {
    dir = (pos2 - pos1);
    dir = dir.norm();
}

MoveByPath::MoveByPath(eVector2f _pos1, eVector2f _pos2, float _speed = 1) {
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
        emitter->setPosition(currPos);
        switched = true;
    }
    else if(currPos.x < std::min(pos1.x, pos2.x)) {
        currPos.x = std::min(pos1.x, pos2.x);
        emitter->setPosition(currPos);
        switched = true;
    }
    if(currPos.y > std::max(pos1.y, pos2.y)) {
        currPos.y = std::max(pos1.y, pos2.y);
        emitter->setPosition(currPos);
        switched = true;
    }
    else if(currPos.y < std::min(pos1.y, pos2.y)) {
        currPos.y = std::min(pos1.y, pos2.y);
        emitter->setPosition(currPos);
        switched = true;
    }

    if(switched)
        dir = -dir;
}

Rotate::Rotate(float _speed) {
    speed = _speed;
}

void Rotate::update(Emitter* emitter) {
    emitter->rotate(speed);
}


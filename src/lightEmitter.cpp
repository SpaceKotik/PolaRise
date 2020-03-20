#include <SFML/Graphics.hpp>

#include "rayTracing.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "lightEmitter.h"

#include "consts.h"
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
}

Emitter::Emitter(const eVector2f _position, const eVector2f _view, bool _updateOnDemand, Color _color, bool _isRestricted) {
    position = _position;
    view = _view;

    isRestricted = _isRestricted;
    lineOfSight = viewAngle;
    updateOnDemand = _updateOnDemand;
    color = _color;
}

Emitter::~Emitter() {

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

///calculcates new VertexArray only if is needed
void Emitter::updateRayTracing(bool update) {
    if (!updateOnDemand || update)
        rayTracing.update(position, isRestricted, view, lineOfSight);

}

VertexArray Emitter::createMesh() {
    return rayTracing.createMesh(color);
}
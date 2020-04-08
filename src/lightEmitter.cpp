#include "emitterBehaviour.h"
#include "lightEmitter.h"

using namespace consts;

#define DEFAULTEMITTER_POS eVector2f(-100, -100)
#define DEFAULTEMITTER_VIEW eVector2f(-100, -100)
#define DEFAULTEMITTER_LINEOFSIGHT 10

Emitter::Emitter() {
    position = DEFAULTEMITTER_POS;
    view = DEFAULTEMITTER_VIEW;
    isRestricted = true;
    lineOfSight = DEFAULTEMITTER_LINEOFSIGHT;
    color = lightColorDefault;
    behaviour = nullptr;
}

Emitter::Emitter(const eVector2f _position, const eVector2f _view, bool _isRestricted, Color _color) {
    position = _position;
    view = _view;
    isRestricted = _isRestricted;
    lineOfSight = viewAngle;
    updateOnDemand = true;
    color = _color;
    behaviour = nullptr;
}

Emitter::~Emitter() = default;


void Emitter::updateLightMap(const RayTracing* _rayTracing) {
    rayTracing = *_rayTracing;
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

///
Vector2f Emitter::getPosition() const {
    return position;
}

void Emitter::setPosition(eVector2f pos) {
    position = pos;
}

void Emitter::setView(eVector2f _view) {
    view = _view;
}

void Emitter::setColor(Color _color) {
    color = _color;
}

void Emitter::setActive(bool _isActive) {
    isActive = _isActive;
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
        behaviour = _behaviour;
        updateOnDemand = false;
}

void Emitter::rotate(const float angle) {
    view.rotate(angle);
}

void Emitter::move(const eVector2f dir) {
    position += dir;
}
#include <SFML/Graphics.hpp>
#include <mutex>
#include <thread>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"
#include "hero.hpp"
#include "dump.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "lightEmitter.h"

extern const float viewAngle;
extern const Color lightColorBlue;
extern const Color lightColorRed;


Emitter::Emitter() {
    position = Vector2f(100, 100);
    view = Vector2f(100, 100);
    isRestricted = true;
    lineOfSight = viewAngle;
    color = lightColorRed;
}

Emitter::Emitter(eVector2f _position, eVector2f _view) {
    position = _position;
    view = _view;
    isRestricted = true;
    lineOfSight = viewAngle;
    //color = lightColorRed;
    color = Color(80, 80, 230);
}

Emitter::~Emitter() {

}

void Emitter::updateLightMap(const RayTracing* _rayTracing) {
    rayTracing = *_rayTracing;
}

void Emitter::setColor(Color _color) {
    color = _color;
}

void Emitter::setPosition(eVector2f pos) {
    position = pos;
}

void Emitter::setView(eVector2f _view) {
    view = _view;
}

void Emitter::setRestricted(bool _isRestricted) {
    isRestricted = _isRestricted;
}

void Emitter::setLineOfSight(float viewAngle) {
    lineOfSight = viewAngle;
}

void Emitter::setStates(Shader &shaderBlur, Shader &shaderShadow, BlendMode blendMode) {
    //shaderBlur.setParameter("dir", dir);
    //shaderBlur.setParameter("image", buffer);
    states.shader = &shaderBlur;
    //states.blendMode = BlendAdd;
}

void Emitter::doRayTracing(RenderTexture *targetTex, std::mutex *rtLock) {

    rayTracing.update(position, true, view, lineOfSight);
    rtLock->lock();
    targetTex->draw(rayTracing.createMesh(color), BlendAdd);
    rtLock->unlock();
}

void Emitter::draw(RenderTexture *targetTex, std::mutex *rtLock) {
    std::thread thr1(&Emitter::doRayTracing, this, targetTex, rtLock);
    thr1.join();
    //doRayTracing(targetTex, rtLock);
}


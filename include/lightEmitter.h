#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"
#include "hero.hpp"
#include "dump.hpp"
#include "../SmartVector2/SmartVector2f.h"

class Emitter {
private:
    RayTracing rayTracing;
    eVector2f position;
    eVector2f view;
    bool isRestricted;
    float lineOfSight;
    Color color;
    RenderStates states;
public:
    Emitter();
    Emitter(eVector2f _position, eVector2f _view);
    ~Emitter();
    void updateLightMap(const RayTracing* _rayTracing);
    void setColor(Color);
    void setPosition(eVector2f);
    void setView(eVector2f);
    void setRestricted(bool);
    void setLineOfSight(float);
    void setStates(Shader &shaderBlur, Shader &shaderShadow, BlendMode blendMode);
    void draw(RenderTexture *texture, std::mutex *rtLock);
    void doRayTracing(RenderTexture *targetTex, std::mutex *rtLock);
};
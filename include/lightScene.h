#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include <unistd.h>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "lightEmitter.h"
#include "level.hpp"
#include "hero.hpp"
#include "dump.hpp"
#include "../SmartVector2/SmartVector2f.h"

class LightScene {
private:
    std::vector<Emitter> scene;
    RayTracing rayTracing;

    Shader shaderBlur;      ///Shaders for enhancing lights
    Shader shaderShadow;    ///
    bool doBlur;            ///If false, shaders are not applied
    bool doShadow;          ///

    RenderTexture targetTex;///RenderTexture, where all lights are drawn to
    void doRayTracing(Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock);  ///The function passed to thread in draw()
public:
    LightScene();
    ~LightScene();

    bool setShaders(bool doBlur, bool doShadow);
    bool updateEmitter(int i, eVector2f pos, eVector2f _view, bool updateObstacles = false);
    void updateEmittersRayTracing(Level *level);     ///Updates obstacles in lightScene rayTracing, then applied to all Emitters
    void addEmitter(eVector2f _position, eVector2f _view, bool updateOnDemand = true);

    bool draw();    ///Draws all Emitters to targetTex

    Texture& getTexture();  ///This texture must be applied to sprite which then drawn to window
};
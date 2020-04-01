#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include <thread>
#include <unistd.h>
#include "rayTracing.hpp"
#include "lightEmitter.h"
#include "level.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "emitterBehaviour.h"

class LightScene {
private:
    std::vector<Emitter> scene;
    RayTracing rayTracing;
    std::vector<EmitterBehaviour::Behaviour*> behaviourPool;

    Shader shaderBlur;      ///Shaders for enhancing lights
    Shader shaderShadow;    ///
    bool doBlur;            ///If false, shaders are not applied
    bool doShadow;          ///

    RenderTexture targetTex;///RenderTexture, where all lights are drawn to
    RenderTexture bufferTex;///RenderTexture for temporal drawing to apply shadow shader

    void doRayTracing(int i, Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock);  ///The function passed to thread in draw()
    bool invalidIndex(int i);
public:
    LightScene();
    ~LightScene();

    bool draw();    ///Draws all Emitters to targetTex
    void update();
    void reset();
    bool setShaders(bool doBlur, bool doShadow);

    void addEmitter(eVector2f position, eVector2f view, EmitterBehaviour::Behaviour* behaviour = nullptr, bool isRestricted = true);
    void deleteEmitter(eVector2f coord);
    bool updateEmitter(int i, eVector2f pos, eVector2f _view, bool updateObstacles = false);
    void updateEmittersRayTracing(Level *level);     ///Updates obstacles in lightScene rayTracing, then applied to all Emitter
    bool setBehaviour(int i, EmitterBehaviour::Behaviour*);

    Texture& getTexture();  ///This texture must be applied to sprite which then drawn to window
    void setActiveTiles(Level *level);
};
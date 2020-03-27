#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include <unistd.h>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "lightEmitter.h"
#include "level.hpp"
#include "player.hpp"
#include "dump.hpp"
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
public:
    LightScene();
    ~LightScene();

    void update();
    bool setShaders(bool doBlur, bool doShadow);
    bool updateEmitter(int i, eVector2f pos, eVector2f _view, bool updateObstacles = false);

    bool setBehaviour(int i, EmitterBehaviour::Behaviour*);
    void updateEmittersRayTracing(Level *level);     ///Updates obstacles in lightScene rayTracing, then applied to all Emitters
    void addEmitter(eVector2f _position, eVector2f _view, bool _isRestricted = true, bool updateOnDemand = true);
    void deleteEmitter(eVector2f coord);
    bool draw();    ///Draws all Emitters to targetTex
    Texture& getTexture();  ///This texture must be applied to sprite which then drawn to window

    void setActiveTiles(Level *level);


};
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

class Game;

class LightScene {
private:
    std::vector<Emitter> scene;
    RayTracing rayTracing;
    std::vector<EmitterBehaviour::Behaviour*> behaviourPool;

    Shader* shaderBlur;      ///Shaders for lights
    Shader* shaderShadow;    ///

    RenderTexture targetTex;///RenderTexture, where all lights are drawn to
    RenderTexture bufferTex;///RenderTexture for temporal drawing to apply shadow shader

    Game* mediator;         ///Reference to game to get access to all required objects like level, resource holders, etc.

protected:
    void doRayTracing(Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock);  ///The function passed to thread in draw()
    bool invalidIndex(int i);
    void applyBlur();
    void setActiveTiles();                  ///sets lighted tiles active
    void removeDeprecatedBehaviours();      ///removes unused behaviours from behaviourPool

public:
    LightScene();
    ~LightScene();
    void setMediator(Game* game);

    Texture& drawToTex();    ///Draws all Emitters to targetTex
    void update();           ///Calls update for emitters behaviour, applies their effects on level's dynamic tiles, etc.
    void reset();
    bool setShaders();

    void addEmitter(eVector2f position, eVector2f view, EmitterBehaviour::Behaviour* behaviour = nullptr, bool isRestricted = true);
    void deleteEmitter(eVector2f coord);
    bool setBehaviour(int i, EmitterBehaviour::Behaviour*);
    void updateEmittersRayTracing();    ///Updates obstacles in lightScene rayTracing, then applied to all Emitters
                                        ///(Must be called only when level has changed)

    Texture& getTexture();  ///This texture must be applied to sprite which then drawn to window
};
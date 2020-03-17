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
    Shader shaderBlur;
    Shader shaderShadow;
    //RenderWindow& window;
    RenderTexture targetTex;

public:
    LightScene();
    //explicit LightScene(RenderWindow& _window);
    ~LightScene();
    bool setShaders(bool doBlur, bool doShadow);
    //bool setWindow(RenderWindow& window);
    bool draw();
    void updateObstacles(Level* level);
    bool updateEmitter(int i, eVector2f pos, eVector2f _view, bool updateObstacles = false);
    void updateEmittersRayTracing();
    void addEmitter(eVector2f _position, eVector2f _view);
    Texture& getTexture();
};
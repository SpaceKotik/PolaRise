#include <SFML/Graphics.hpp>
#include <mutex>
#include "tile.hpp"
#include "rayTracing.hpp"
#include "level.hpp"
#include "hero.hpp"
#include "dump.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "lightEmitter.h"
#include "lightScene.h"

extern const float viewAngle;
extern const Color lightColorBlue;
extern const Color lightColorRed;


LightScene::LightScene() {
    setShaders(false, false);
    targetTex.create(30*40, 30*30);
    targetTex.setSmooth(true);
}

/*LightScene::LightScene(RenderWindow& _window) : window(_window){
    setShaders(false, false);
    targetTex.create(window.getSize().x, window.getSize().y);
    targetTex.setSmooth(true);
}*/

LightScene::~LightScene() {

}

bool LightScene::setShaders(bool doBlur, bool doShadow) {
    if(doShadow) {
        if(!shaderShadow.loadFromFile("../shaders/shadow.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderShadow.setParameter("frag_ScreenResolution", Vector2f(1200, 900));
        shaderShadow.setParameter("frag_LightColor", Vector3f(255, 255, 255));
    }

    if(doBlur) {
        if(!shaderBlur.loadFromFile("../shaders/blur.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderBlur.setParameter("resolution", Vector2f(1200, 900));
    }

    return true;
}

bool LightScene::draw() {
    targetTex.clear(Color::Black);
    std::mutex block;
    for (auto e: scene) {
        e.draw(&targetTex, &block); ///!!!
    }
    targetTex.display();
    return true;
}
/*
bool LightScene::setWindow(RenderWindow& _window) {
    //window = &window;
}*/

void LightScene::updateObstacles(Level *level) {
    rayTracing.updateObstacles(level);
}

bool LightScene::updateEmitter(int i, eVector2f pos, eVector2f view, bool updateObstacles) {
    if (updateObstacles)
        scene.at(i).updateLightMap(&rayTracing);
    scene.at(i).setPosition(pos);
    scene.at(i).setView(view);
}

void LightScene::updateEmittersRayTracing() {
    for (auto &e : scene) {
        e.updateLightMap(&rayTracing);
    }
}

void LightScene::addEmitter(eVector2f _position, eVector2f _view) {
    Emitter emitter(_position, _view);
    scene.push_back(emitter);
}

Texture& LightScene::getTexture() {
    return const_cast<Texture &>(targetTex.getTexture());
}
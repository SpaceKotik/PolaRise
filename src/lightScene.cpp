#include <SFML/Graphics.hpp>
#include <mutex>
#include <thread>
#include "level.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "lightEmitter.h"
#include "lightScene.h"

#include "consts.h"
using namespace consts;

LightScene::LightScene() {
    doBlur = true;
    doShadow = true;
    setShaders(doBlur, doShadow);
    targetTex.create(window_x, window_y);
    bufferTex.create(window_x, window_y);
    targetTex.setSmooth(true);
}

LightScene::~LightScene() {

}

///function passed to threads
void LightScene::doRayTracing(int i, Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock) {
    emitter.updateRayTracing();

    rtLock.lock();
    //targetTex.draw(emitter.createMesh(), BlendAdd);
    RenderStates states;

    states.blendMode = BlendAdd;
    if(doShadow) {
    shaderShadow.setUniform("frag_LightOrigin", scene.at(i).getPosition());
    shaderShadow.setUniform("frag_ShadowParam1", float(20000));
    shaderShadow.setUniform("frag_ShadowParam2", float(20000));
    shaderShadow.setUniform("frag_LightColor", Vector3f(80, 80, 230));
    //shaderShadow.setUniform("frag_LightColor", Vector3f(250, 245, 245));
    states.shader = &shaderShadow;
    }

    targetTex.draw(emitter.createMesh(), states);
    targetTex.setSmooth(true);
    targetTex.display();

    rtLock.unlock();
}

bool LightScene::setShaders(bool _doBlur, bool _doShadow) {
    doBlur = _doBlur;
    doShadow = _doShadow;

    if(doBlur) {
        if(!shaderBlur.loadFromFile("../shaders/blur.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderBlur.setUniform("resolution", Vector2f(window_x, window_y));
    }
    if(doShadow) {
        if(!shaderShadow.loadFromFile("../shaders/shadow.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderShadow.setUniform("frag_ScreenResolution", Vector2f(window_x, window_y));
        shaderShadow.setUniform("frag_LightColor", Vector3f(255, 255, 255));
    }

    return true;
}

bool LightScene::updateEmitter(int i, eVector2f pos, eVector2f view, bool updateObstacles) {
    if (updateObstacles)
        scene.at(i).updateLightMap(&rayTracing);
    scene.at(i).setPosition(pos);
    scene.at(i).setView(view);
}

void LightScene::updateEmittersRayTracing(Level *level) {
    rayTracing.updateObstacles(level);
    for (auto &e : scene) {
        e.updateLightMap(&rayTracing);
        e.updateRayTracing(true);
    }
}

void LightScene::addEmitter(eVector2f _position, eVector2f _view, bool _isRestricted, bool updateOnDemand) {
    Emitter emitter(_position, _view, updateOnDemand,Color(80, 80, 230), _isRestricted);
    scene.push_back(emitter);
}

void LightScene::deleteEmitter(eVector2f coord) {

        scene.erase(std::remove_if(scene.begin() + 1, scene.end(), [coord](Emitter const &emitter)-> bool {
            eVector2f diff = emitter.getPosition() - coord;
            if (diff.len() < 50)
                return true;
            else
                return false;
        }), scene.end());
}

bool LightScene::draw() {
    targetTex.clear(Color::Black);

    /// draw Emitters in threads
    std::vector<std::thread> drawThreads;
    std::mutex block;
    int index = 0;
    for (auto &e: scene) {
        drawThreads.emplace_back(std::thread(&LightScene::doRayTracing, this, index, std::ref(e), std::ref(targetTex), std::ref(block))); ///changed from push_back
        index++;
    }
    for (auto &e: drawThreads) {
        e.join();
    }

    ///temporal vars for applying shaders
    Sprite bufferSprite;
    RenderStates states;
    bufferSprite.setTexture(targetTex.getTexture());
    bufferSprite.setOrigin(window_x  / 2.f, window_y / 2.f);
    bufferSprite.setPosition(window_x / 2.f, window_y / 2.f);
    //blur all light
    if (doBlur) {
        shaderBlur.setUniform("image", targetTex.getTexture());

        for (int i = 0; i < 1; ++i) {


            shaderBlur.setUniform("dir", Vector2f(1, 0));

            states.shader = &shaderBlur;

            shaderBlur.setUniform("dir", Vector2f(0, 1));
            targetTex.draw(bufferSprite, states);
            targetTex.display();

            shaderBlur.setUniform("dir", Vector2f(1, 1));
            targetTex.draw(bufferSprite, states);
            targetTex.display();

            shaderBlur.setUniform("dir", Vector2f(1, -1));
            targetTex.draw(bufferSprite, states);
            targetTex.display();
        }
    }

    ;
    ///may be used to shadow everything except player
    //make shadow
    /*if (doShadow) {///
        states.blendMode = BlendMultiply;
        shaderShadow.setUniform("frag_LightOrigin", scene.at(0).getPosition());
        shaderShadow.setUniform("frag_ShadowParam1", float(20000));
        shaderShadow.setUniform("frag_ShadowParam2", float(20000));
        states.shader = &shaderShadow;

        targetTex.draw(bufferSprite, states);
        targetTex.setSmooth(true);
        targetTex.display();
    }*/
    return true;
}

Texture& LightScene::getTexture() {
    return const_cast<Texture &>(targetTex.getTexture());
}
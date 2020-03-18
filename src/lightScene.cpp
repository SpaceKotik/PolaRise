#include <SFML/Graphics.hpp>
#include <mutex>
#include <thread>

#include "level.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "lightEmitter.h"
#include "lightScene.h"

extern const float viewAngle;
extern const Color lightColorBlue;
extern const Color lightColorRed;
extern const int field_x;
extern const int field_y;
extern const int scale;

LightScene::LightScene() {
    doBlur = true;
    doShadow = true;
    setShaders(doBlur, doShadow);
    targetTex.create(field_x*scale, field_y*scale);
    targetTex.setSmooth(true);
}

LightScene::~LightScene() {

}

void LightScene::doRayTracing(Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock) {
    emitter.updateRayTracing();
    rtLock.lock();
    targetTex.draw(emitter.createMesh(), BlendAdd);
    rtLock.unlock();
}

bool LightScene::setShaders(bool _doBlur, bool _doShadow) {
    doBlur = _doBlur;
    doShadow = _doShadow;

    if(doBlur) {
        if(!shaderBlur.loadFromFile("../shaders/blur.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderBlur.setUniform("resolution", Vector2f(field_x*scale, field_y*scale));
    }
    if(doShadow) {
        if(!shaderShadow.loadFromFile("../shaders/shadow.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderShadow.setUniform("frag_ScreenResolution", Vector2f(field_x*scale, field_y*scale));
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

void LightScene::addEmitter(eVector2f _position, eVector2f _view, bool updateOnDemand) {
    Emitter emitter(_position, _view, updateOnDemand);
    scene.push_back(emitter);
}

bool LightScene::draw() {
    targetTex.clear(Color::Black);
    std::mutex block;

    std::vector<std::thread> drawThreads;

    for (auto &e: scene) {
        drawThreads.push_back(std::thread(&LightScene::doRayTracing, this, std::ref(e), std::ref(targetTex), std::ref(block)));
    }
    for (auto &e: drawThreads) {
        e.join();
    }

    //targetTex.display();


    //blur all light
    if (doBlur) {
        Sprite bufferSprite;
        RenderStates states;
        bufferSprite.setTexture(targetTex.getTexture());
        bufferSprite.setOrigin(field_x * scale / 2.f, field_y * scale / 2.f);
        bufferSprite.setPosition(field_x * scale / 2.f, field_y * scale / 2.f);

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
    //make shadow
    if (!doShadow) {///
        Sprite bufferSprite;
        RenderStates states;
        bufferSprite.setTexture(targetTex.getTexture());
        bufferSprite.setOrigin(field_x * scale / 2.f, field_y * scale / 2.f);
        bufferSprite.setPosition(field_x * scale / 2.f, field_y * scale / 2.f);

        states.blendMode = BlendMultiply;
        shaderShadow.setUniform("frag_ScreenResolution", Vector2f(field_x*scale, field_y*scale));
        shaderShadow.setUniform("frag_LightColor", Vector3f(255, 255, 255));
        shaderShadow.setUniform("frag_LightOrigin", Vector2f(100, 100));
        shaderShadow.setUniform("frag_ShadowParam1", 20000);
        shaderShadow.setUniform("frag_ShadowParam2", 20000);
        states.shader = &shaderShadow;

        //targetTex.setActive(true);
        targetTex.draw(bufferSprite, states);
        targetTex.setSmooth(true);
        targetTex.display();
    }
    return true;
}

Texture& LightScene::getTexture() {
    return const_cast<Texture &>(targetTex.getTexture());
}


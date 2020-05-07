#include "lightScene.h"
#include "consts.h"
#include "game.hpp"
#include "util/ResourceHolders/ShaderHolder/shaderHolder.h"
#include "util/ResourceHolders/resourceIdentifiers.h"
#include "Game/sceneLoader.h"
#include <typeinfo>

using namespace consts;

// TODO: fix loading shaders
LightScene::LightScene() {
    targetTex.create(windowSize.x, windowSize.y);
    bufferTex.create(windowSize.x, windowSize.y);
    targetTex.setSmooth(true);
    sceneLoader.setLightScene(this);
    mediator = nullptr;
}

LightScene::~LightScene() = default;

void LightScene::setMediator(Game* _game) {
    mediator = _game;
}

void LightScene::update() {
    updateEmittersRayTracing();
    ///reset activated tiles from previous update call
    mediator->getLevel()->resetActive();
    ///and set new
    setActiveTiles();
    ///set tile colors according to their status
    mediator->getLevel()->update();
    ///free unused pointers to behaviours
    removeDeprecatedBehaviours();
    removeShot();
}

Texture& LightScene::drawToTex() {

    targetTex.clear(Color::Black);

    // TODO: make drawThreads a class field
    ///Draw Emitters in threads
    std::vector<std::thread> drawThreads;
    std::mutex block;
    for (auto &e: scene) {
        if (e.isActive) {
            drawThreads.emplace_back(
                    std::thread(&LightScene::doRayTracing, this, std::ref(e), std::ref(targetTex),
                                std::ref(block))); ///changed from push_back
        }
    }
    for (auto &e: drawThreads) {
        e.join();
    }

    ///Set shadow shader
    RenderStates states;
    states.blendMode= BlendAdd;
    if(DOSHADOW) {
        mediator->getShaders()->setParam(Shaders::Shadow, "frag_LightColor", Vector3f(52, 125, 125));
        mediator->getShaders()->setParam(Shaders::Shadow, "coef1", 140.f);
        mediator->getShaders()->setParam(Shaders::Shadow, "coef2", 0.07f);
        mediator->getShaders()->setParam(Shaders::Shadow, "coef3", 0.017f);
        states.shader = shaderShadow;
    }

    ///Draw all lights (Not in threads as OpenGL does not support multi-threading and may not work (e.g. on win10)
    for (auto &e: scene) {
        ///If current emitter is inactive, skip it
        if (!e.isActive)
            continue;
        if(DOSHADOW) {
            // TODO: make shader color constant (or apply color not in shader)
            mediator->getShaders()->setParam(Shaders::Shadow, "frag_LightOrigin", e.getPosition());
        }
        // TODO:: make createMesh() store the mesh in RayTracing to make it possible to create mesh in threads
        targetTex.draw(e.createMesh(), states);
    }
    targetTex.display();

    ///Apply blur to the image (smooths border between pixelated and non-pixelated light)
    if (DOBLUR)
        applyBlur();
    return const_cast<Texture &>(targetTex.getTexture());
}

void LightScene::reset() {
    scene.clear();
}

bool LightScene::setShaders() {

    if(DOBLUR) {
        shaderBlur = mediator->getShaders()->get(Shaders::Blur);
        shaderBlur->setUniform("resolution", Vector2f(windowSize.x, windowSize.y));
    }
    if(DOSHADOW) {
        shaderShadow = mediator->getShaders()->get(Shaders::Shadow);
        shaderShadow->setUniform("frag_ScreenResolution", Vector2f(windowSize.x, windowSize.y));
        shaderShadow->setUniform("frag_LightColor", Vector3f(255, 255, 255));
    }

    return true;
}

void LightScene::addEmitter(eVector2f position, eVector2f view, EmitterBehaviour::Behaviour* behaviour, bool isRestricted) {
    ///updateOmDemand must be smart
    Emitter emitter(position, view, isRestricted, lightDefaultColor);
    if (behaviour != nullptr) {
        emitter.setBehaviour(behaviour);
        behaviourPool.push_back(behaviour);
    }
    scene.push_back(emitter);
    updateEmittersRayTracing(true);
}

void LightScene::addEmitterOnInput(eVector2f position) {
    Emitter emitter(position, eVector2f(0, 1), true, lightDefaultColor);
    scene.push_back(emitter);
    updateEmittersRayTracing(true);
    sceneLoader.loadToFile(position);
}

void LightScene::deleteEmitter(eVector2f coord) {

    scene.erase(std::remove_if(scene.begin(), scene.end(), [coord](Emitter const &emitter)-> bool {
        eVector2f diff = emitter.getPosition() - coord;
        return diff.len() < 50;
    }), scene.end());
}

void LightScene::updateEmittersRayTracing(bool force) {
    if (force)
        rayTracing.updateObstacles(mediator->getLevel());
    for (auto &e : scene) {
        if (force)
            e.updateLightMap(&rayTracing);
        e.updateRayTracing(force);
    }
}

bool LightScene::setBehaviour(int i, EmitterBehaviour::Behaviour* _behaviour) {
    if (invalidIndex(i))
        return false;
    scene.at(i).setBehaviour(_behaviour);
    behaviourPool.push_back(_behaviour);
    return true;
}


bool LightScene::invalidIndex(int i) {
    if (i >= scene.size()) {
        std::cout << "Error: invalid emitter index\n";
        return true;
    }
    return false;
}

///function passed to threads
void LightScene::doRayTracing(Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock) {
    emitter.updateRayTracing();
    //TODO: delete this
/*
    rtLock.lock();

    RenderStates states;
    states.blendMode = BlendAdd;
    if(false) {
        // TODO: make shader color constant (or apply color not in shader)
        mediator->getShaders()->setParam(Shaders::Shadow, "frag_LightOrigin", emitter.getPosition());
        mediator->getShaders()->setParam(Shaders::Shadow, "frag_LightColor", Vector3f(52, 125, 125));
        mediator->getShaders()->setParam(Shaders::Shadow, "coef1", 140.f);
        mediator->getShaders()->setParam(Shaders::Shadow, "coef2", 0.07f);
        mediator->getShaders()->setParam(Shaders::Shadow, "coef3", 0.017f);
        states.shader = shaderShadow;
    }

    //targetTex.draw(emitter.createMesh(), states);
    //targetTex.setSmooth(true);
    //targetTex.display();

    rtLock.unlock();*/
}

void LightScene::removeDeprecatedBehaviours() {
    for (auto &e : scene) {
        e.update();
    }
    while (behaviourPool.size() > scene.size()) {
        for (auto e = behaviourPool.begin() ; e != behaviourPool.end(); ) {
            bool isOutdated = true;
            for (const auto& k : scene) {
                if (k.behaviour == *e) {
                    isOutdated = false;
                    break;
                }
            }
            if (isOutdated) {
                delete *e;
                e = behaviourPool.erase(e);
            }
            else
                ++e;
        }
    }
}

void LightScene::removeShot() {
    scene.erase(std::remove_if(scene.begin(), scene.end(), [](Emitter const &emitter)-> bool {
        if (emitter.behaviour == nullptr)
            return false;
        return emitter.behaviour->mustBeDeleted();
    }), scene.end());


}

void LightScene::removeShotForce() {
    scene.erase(std::remove_if(scene.begin(), scene.end(), [](Emitter const &emitter)-> bool {
        if (emitter.behaviour == nullptr)
            return false;
        return typeid(EmitterBehaviour::Shootable) == typeid(*emitter.behaviour);
    }), scene.end());
}

void LightScene::setActiveTiles() {
    for (auto &e : scene) {
        if(e.isActive)
            e.setActiveTiles(mediator->getLevel());
    }
}

void LightScene::applyBlur() {

    ///temporal vars for applying shader
    Sprite bufferSprite;
    RenderStates states;
    bufferSprite.setTexture(bufferTex.getTexture());
    bufferSprite.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSprite.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);

    ///blur all light
    bufferTex.clear();

    states.shader = shaderBlur;

    shaderBlur->setUniform("image", targetTex.getTexture());
    shaderBlur->setUniform("dir", Vector2f(1, 0));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();

    shaderBlur->setUniform("image", bufferTex.getTexture());
    shaderBlur->setUniform("dir", Vector2f(0, 1));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();

    shaderBlur->setUniform("image", bufferTex.getTexture());
    shaderBlur->setUniform("dir", Vector2f(1, 1));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();

    shaderBlur->setUniform("image", bufferTex.getTexture());
    shaderBlur->setUniform("dir", Vector2f(1, -1));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();


    bufferSprite.setTexture(targetTex.getTexture());

    shaderBlur->setUniform("image", targetTex.getTexture());
    shaderBlur->setUniform("dir", Vector2f(1, 0));
    targetTex.draw(bufferSprite, states);
    targetTex.display();

    shaderBlur->setUniform("image", targetTex.getTexture());
    shaderBlur->setUniform("dir", Vector2f(0, 1));
    targetTex.draw(bufferSprite, states);
    targetTex.display();


    bufferSprite.setTexture(bufferTex.getTexture());
    targetTex.draw(bufferSprite, BlendAdd);
    targetTex.display();
}

Texture& LightScene::getTexture() {
    return const_cast<Texture &>(targetTex.getTexture());
}

Player *LightScene::getPlayer() {
    return mediator->getPlayer();
}

std::string LightScene::getLevelPath() {
    return mediator->getLevelPath();
}

void LightScene::loadScene() {
    sceneLoader.loadFromFile();
}

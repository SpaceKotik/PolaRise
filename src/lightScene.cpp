#include "lightScene.h"
#include "consts.h"
#include "game.hpp"

using namespace consts;

// TODO: fix loading shaders
LightScene::LightScene() {
    doBlur = DOBLUR;
    doShadow = DOSHADOW;
    /// TODO: check if shaders actually loaded
    setShaders(DOBLUR, DOSHADOW);
    ///
    targetTex.create(windowSize.x, windowSize.y);
    bufferTex.create(windowSize.x, windowSize.y);
    targetTex.setSmooth(true);
    mediator = nullptr;
}

LightScene::~LightScene() = default;

void LightScene::setMediator(Game* _game) {
    mediator = _game;
}

void LightScene::update() {
    ///reset activated tiles from previous update call
    mediator->getLevel()->resetActive();
    ///and set new
    setActiveTiles();
    ///set tile colors according to their status
    mediator->getLevel()->update();
    ///free unused pointers to behaviours
    removeDeprecatedBehaviours();
}

Texture& LightScene::drawToTex() {
    targetTex.clear(Color::Black);

    /// draw Emitters in threads
    std::vector<std::thread> drawThreads;
    std::mutex block;
    int index = 0;
    for (auto &e: scene) {
        if (e.isActive) {
            drawThreads.emplace_back(
                    std::thread(&LightScene::doRayTracing, this, index, std::ref(e), std::ref(targetTex),
                                std::ref(block))); ///changed from push_back
            index++;
        }
    }
    for (auto &e: drawThreads) {
        e.join();
    }
    if (doBlur)
        applyBlur();
    return const_cast<Texture &>(targetTex.getTexture());
}

void LightScene::reset() {
    scene.clear();
}

bool LightScene::setShaders(bool _doBlur, bool _doShadow) {
    doBlur = _doBlur;
    doShadow = _doShadow;

    if(doBlur) {
        if(!shaderBlur.loadFromFile("../shaders/blur.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderBlur.setUniform("resolution", Vector2f(windowSize.x, windowSize.y));
    }
    if(doShadow) {
        if(!shaderShadow.loadFromFile("../shaders/shadow.frag", sf::Shader::Fragment)) {
            return false;
        }
        shaderShadow.setUniform("frag_ScreenResolution", Vector2f(windowSize.x, windowSize.y));
        shaderShadow.setUniform("frag_LightColor", Vector3f(255, 255, 255));
    }

    return true;
}



void LightScene::addEmitter(eVector2f position, eVector2f view, EmitterBehaviour::Behaviour* behaviour, bool isRestricted) {
    ///updateOmDemand must be smart
    Emitter emitter(position, view, isRestricted, lightColorDefault);
    if (behaviour != nullptr) {
        emitter.setBehaviour(behaviour);
        behaviourPool.push_back(behaviour);
    }
    scene.push_back(emitter);
    updateEmittersRayTracing();
}

void LightScene::deleteEmitter(eVector2f coord) {

    scene.erase(std::remove_if(scene.begin(), scene.end(), [coord](Emitter const &emitter)-> bool {
        eVector2f diff = emitter.getPosition() - coord;
        return diff.len() < 50;
    }), scene.end());
}

void LightScene::updateEmittersRayTracing() {
    rayTracing.updateObstacles(mediator->getLevel());
    for (auto &e : scene) {
        e.updateLightMap(&rayTracing);
        e.updateRayTracing(true);
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
void LightScene::doRayTracing(int i, Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock) {
    emitter.updateRayTracing();

    rtLock.lock();

    RenderStates states;
    states.blendMode = BlendAdd;
    if(doShadow) {
        shaderShadow.setUniform("frag_LightOrigin", emitter.getPosition());
        //shaderShadow.setUniform("frag_LightColor", Vector3f(255, 210, 80));
        //shaderShadow.setUniform("frag_LightColor", Vector3f(125, 105, 40));
        // TODO: make shader color constant (or apply color not in shader)
        shaderShadow.setUniform("frag_LightColor", Vector3f(52, 125, 125));
        states.shader = &shaderShadow;
    }

    targetTex.draw(emitter.createMesh(), states);
    targetTex.setSmooth(true);
    targetTex.display();

    rtLock.unlock();
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

    states.shader = &shaderBlur;

    shaderBlur.setUniform("image", targetTex.getTexture());
    shaderBlur.setUniform("dir", Vector2f(1, 0));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();

    shaderBlur.setUniform("image", bufferTex.getTexture());
    shaderBlur.setUniform("dir", Vector2f(0, 1));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();

    shaderBlur.setUniform("image", bufferTex.getTexture());
    shaderBlur.setUniform("dir", Vector2f(1, 1));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();

    shaderBlur.setUniform("image", bufferTex.getTexture());
    shaderBlur.setUniform("dir", Vector2f(1, -1));
    bufferTex.draw(bufferSprite, states);
    bufferTex.display();


    bufferSprite.setTexture(targetTex.getTexture());

    /*shaderBlur.setUniform("image", targetTex.getTexture());
    shaderBlur.setUniform("dir", Vector2f(1, 0));
    targetTex.draw(bufferSprite, states);
    targetTex.display();

    shaderBlur.setUniform("image", targetTex.getTexture());
    shaderBlur.setUniform("dir", Vector2f(0, 1));
    targetTex.draw(bufferSprite, states);
    targetTex.display();*/


    bufferSprite.setTexture(bufferTex.getTexture());
    targetTex.draw(bufferSprite, BlendAdd);
    targetTex.display();
}

Texture& LightScene::getTexture() {
    return const_cast<Texture &>(targetTex.getTexture());
}
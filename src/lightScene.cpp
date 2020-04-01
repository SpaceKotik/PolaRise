#include "lightScene.h"
#include "consts.h"

using namespace consts;

// TODO: fix loading shaders
LightScene::LightScene() {
    doBlur = true;
    doShadow = true;
    setShaders(doBlur, doShadow);
    targetTex.create(windowSize.x, windowSize.y);
    bufferTex.create(windowSize.x, windowSize.y);
    targetTex.setSmooth(true);
}

LightScene::~LightScene() = default;

bool LightScene::draw() {
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

    ///temporal vars for applying shaders
    Sprite bufferSprite;
    RenderStates states;
    bufferSprite.setTexture(targetTex.getTexture());
    bufferSprite.setOrigin(windowSize.x / 2.f, windowSize.y / 2.f);
    bufferSprite.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    ///blur all light
    if (doBlur) {
        shaderBlur.setUniform("image", targetTex.getTexture());

        for (int i = 0; i < 1; ++i) {
            bufferSprite.setTexture(bufferTex.getTexture());
            states.shader = &shaderBlur;

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
        }
        bufferSprite.setTexture(bufferTex.getTexture());
        targetTex.draw(bufferSprite, BlendAdd);
        targetTex.draw(bufferSprite, BlendAdd);
        targetTex.display();

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

void LightScene::update() {
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

bool LightScene::updateEmitter(int i, eVector2f pos, eVector2f view, bool updateObstacles) {
    if (invalidIndex(i))
        return false;
    if (updateObstacles)
        scene.at(i).updateLightMap(&rayTracing);
    scene.at(i).setPosition(pos);
    scene.at(i).setView(view);
    return true;
}

void LightScene::updateEmittersRayTracing(Level *level) {
    rayTracing.updateObstacles(level);
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


Texture& LightScene::getTexture() {
    return const_cast<Texture &>(targetTex.getTexture());
}
void LightScene::setActiveTiles(Level *level) {
    for (auto &e : scene) {
        if(e.isActive)
            e.setActiveTiles(level);
    }
}


bool LightScene::invalidIndex(int i) {
    if (i >=scene.size()) {
        std::cout << "Error: invalid emitter index\n";
        return false;
    }
    return true;
}
///function passed to threads
void LightScene::doRayTracing(int i, Emitter &emitter, RenderTexture &_targetTex, std::mutex &rtLock) {
    emitter.updateRayTracing();

    rtLock.lock();

    RenderStates states;
    states.blendMode = BlendAdd;
    if(doShadow) {
        shaderShadow.setUniform("frag_LightOrigin", emitter.getPosition());
        //shaderShadow.setUniform("frag_ShadowParam1", float(20000));
        //shaderShadow.setUniform("frag_ShadowParam2", float(20000));
        //shaderShadow.setUniform("frag_LightColor", Vector3f(255, 210, 80));
        shaderShadow.setUniform("frag_LightColor", Vector3f(125, 105, 40));
        //shaderShadow.setUniform("frag_LightColor", Vector3f(250, 245, 245));
        states.shader = &shaderShadow;
    }

    targetTex.draw(emitter.createMesh(), states);
    targetTex.setSmooth(true);
    targetTex.display();

    rtLock.unlock();
}


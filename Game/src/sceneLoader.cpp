#include <fstream>
#include "Game/include/sceneLoader.h"
#include "util/SmartVector2/SmartVector2f.h"
#include "LightSystem/include/lightScene.h"


/// Used for convenient interpretation of file commands
std::map <std::string, int> behaviourNamesMapping {
        {"NoBehaviour", 0},
        {"MoveByPath", 1},
        {"Rotate", 2},
        {"MoveByCircle", 3},
        {"Flicker", 4},
        {"BindToPlayer", 5},
};

bool SceneLoader::loadToFile(eVector2f pos) {
    std::ofstream levelFile(lightScene->getLevelPath() + "lightScene.txt", std::ios::app);
    if(!levelFile.is_open())
        return false;
    levelFile << "NoBehaviour " << pos.x << " " << pos.y << std::endl;
    return true;
}

bool SceneLoader::loadFromFile() {
    std::ifstream levelFile(lightScene->getLevelPath() + "lightScene.txt");
    if(!levelFile.is_open())
        return false;

    std::string currFeature;
    while (levelFile >> currFeature) {
        switch (behaviourNamesMapping[currFeature]) {
            case 0:
                handleNoBehaviour(&levelFile);
                break;
            case 1:
                handleMoveByPath(&levelFile);
                break;
            case 2:
                handleRotate(&levelFile);
                break;
            case 3:
                handleMoveByCircle(&levelFile);
                break;
            case 4:
                handleFlicker(&levelFile);
                break;
            case 5:
                handleBindToPlayer(&levelFile);
                break;
            default:
                break;
        }
    }
    return true;
}

bool SceneLoader::setLightScene(LightScene *_lightScene) {
    if (lightScene == nullptr)
        return false;
    lightScene = _lightScene;
    return true;
}

void SceneLoader::handleMoveByPath(std::ifstream* levelFile) {
    eVector2f pos1, pos2, view;
    float speed;
    *levelFile >> view.x;
    *levelFile >> view.y;
    *levelFile >> pos1.x;
    *levelFile >> pos1.y;
    *levelFile >> pos2.x;
    *levelFile >> pos2.y;
    *levelFile >> speed;
    lightScene->addEmitter(eVector2f(0, 0), view, new EmitterBehaviour::MoveByPath(pos1, pos2, speed));
}

void SceneLoader::handleRotate(std::ifstream* levelFile) {
    eVector2f position, view;
    float rotateSpeed;
    *levelFile >> position.x;
    *levelFile >> position.y;
    *levelFile >> view.x;
    *levelFile >> view.y;
    *levelFile >> rotateSpeed;
    lightScene->addEmitter(position, view, new EmitterBehaviour::Rotate(rotateSpeed));

}

void SceneLoader::handleMoveByCircle(std::ifstream* levelFile) {
    eVector2f center;
    float radius;
    float rotateSpeed;
    *levelFile >> center.x;
    *levelFile >> center.y;
    *levelFile >> radius;
    *levelFile >> rotateSpeed;
    lightScene->addEmitter(eVector2f(0, 0), eVector2f(0, 1), new EmitterBehaviour::MoveByCircle(center, radius, rotateSpeed));
}

void SceneLoader::handleFlicker(std::ifstream* levelFile) {
    eVector2f position, view;
    float period;
    *levelFile >> position.x;
    *levelFile >> position.y;
    *levelFile >> view.x;
    *levelFile >> view.y;
    *levelFile >> period;
    lightScene->addEmitter(position, view, new EmitterBehaviour::Flicker(period));
}

void SceneLoader::handleBindToPlayer(std::ifstream* levelFile) {
    lightScene->addEmitter(eVector2f(0, 0), eVector2f(0, 0), new EmitterBehaviour::BindToPlayer(lightScene->getPlayer()));
}

void SceneLoader::handleNoBehaviour(std::ifstream *levelFile) {
    eVector2f position;
    *levelFile >> position.x;
    *levelFile >> position.y;
    lightScene->addEmitter(position, eVector2f(0, 1), nullptr);
}
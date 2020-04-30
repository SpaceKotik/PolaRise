#pragma once
#include <iostream>

class eVector2f;
class LightScene;

class SceneLoader {
public:
    LightScene* lightScene;
private:
    void handleNoBehaviour(std::ifstream*);
    void handleMoveByPath(std::ifstream*);
    void handleRotate(std::ifstream*);
    void handleMoveByCircle(std::ifstream*);
    void handleFlicker(std::ifstream*);
    void handleBindToPlayer(std::ifstream*);
public:
    bool setLightScene(LightScene* lightScene);
    bool loadToFile(eVector2f pos);
    bool loadFromFile();
};


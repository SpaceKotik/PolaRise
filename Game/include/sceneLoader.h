#pragma once
#include <iostream>

class eVector2f;
class LightScene;

class SceneLoader {
private:
    LightScene* lightScene;
private:
    void handleNoBehaviour(std::ifstream*);         /// All of these handle different types of strings describing emitters with different behaviours
    void handleMoveByPath(std::ifstream*);
    void handleRotate(std::ifstream*);
    void handleMoveByCircle(std::ifstream*);
    void handleFlicker(std::ifstream*);
    void handleBindToPlayer(std::ifstream*);
public:
    bool setLightScene(LightScene* lightScene);
    bool loadToFile(eVector2f pos);                 /// Loads the position of the emitter placed by user with NoBehaviour prefix
                                                    /// for convenient level editing (after that this string in level scene file
                                                    /// may be easily modified)
    bool loadFromFile();
};
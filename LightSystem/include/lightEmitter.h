#pragma once

#include <algorithm>
#include "rayTracing.hpp"
#include "Game/include/level.hpp"
#include "util/SmartVector2/SmartVector2f.h"
#include "Game/include/consts.h"

namespace EmitterBehaviour {
    class Behaviour;
    class Rotate;
    class MoveByPath;
    class MoveByCircle;
    class Flicker;
}


class Emitter {
private:
    RayTracing rayTracing;
    eVector2f position;
    eVector2f view;             /// vector defining the direction of view
    bool isRestricted;          /// if false, lineOfSight is 2*PI
    float lineOfSight;          /// angle size of light source
    bool updateOnDemand = true; /// Recalculate vertex array for mesh only if updateRayTracing() called with 'true'
    bool isActive = true;       /// Defines if this emitter will interact with level and then displayed
    Color color;
    EmitterBehaviour::Behaviour *behaviour = nullptr;
public:
    Emitter();
    Emitter(eVector2f position, eVector2f view, bool isRestricted = true, Color color = consts::lightDefaultColor);
    ~Emitter();

    void updateLightMap(const RayTracing* _rayTracing); /// sets RayTracing equal to lightScene RayTracing (basically all obstacles)
    void update();                                      /// updates behaviour

    void setActiveTiles(Level *level);                  /// this function just passes call to RayTracing method
    void updateRayTracing(bool update = false);         /// updates light mesh (should be called only if emitters position, view, etc. changes)
    VertexArray createMesh();


    Vector2f getPosition() const;
    void setPosition(eVector2f);
    void setView(eVector2f);
    void setColor(Color);
    void setRestricted(bool);
    bool setLineOfSight(float);
    void setActive(bool);
    void setBehaviour(EmitterBehaviour::Behaviour*);

    void rotate(float angle);
    void move(eVector2f dir);

    friend class LightScene;
};
#pragma once

#include <algorithm>
#include "rayTracing.hpp"
#include "level.hpp"
#include "../SmartVector2/SmartVector2f.h"
#include "consts.h"

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
    bool isActive = true;
    Color color;
    EmitterBehaviour::Behaviour *behaviour = nullptr;
public:
    Emitter();
    Emitter(eVector2f _position, eVector2f _view, bool _updateOnDemand = true, Color _color = Color(80, 80, 230), bool _isRestricted = true);
    ~Emitter();
    void updateLightMap(const RayTracing* _rayTracing); /// sets RayTracing equal to lightScene RayTracing (basically all obstacles)

    void setPosition(eVector2f);
    Vector2f getPosition() const;
    void setView(eVector2f);
    void setColor(Color);
    void setRestricted(bool);
    bool setLineOfSight(float);
    void setActive(bool);
    void setBehaviour(EmitterBehaviour::Behaviour*);
    void rotate(float angle);
    void move(eVector2f dir);

    void update();
    void setActiveTiles(Level *level);
    void updateRayTracing(bool update = false);
    VertexArray createMesh();
    friend class LightScene;
};
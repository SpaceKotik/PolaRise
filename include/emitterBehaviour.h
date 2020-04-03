#pragma once

#include <SFML/Graphics.hpp>
#include "../SmartVector2/SmartVector2f.h"
class Emitter;
class Player;

namespace EmitterBehaviour {

    class Behaviour {
    public:
        Behaviour();
        virtual ~Behaviour() = 0;
        virtual void update(Emitter* emitter){};
    };

    class MoveByPath : public Behaviour {
    private:
        eVector2f pos1 = {100, 100};
        eVector2f pos2 = {100, 200};
        eVector2f dir;
        float speed = 1;

    public:
        MoveByPath();
        MoveByPath(eVector2f, eVector2f, float);
        void update(Emitter*) override;
    };

    class Rotate : public Behaviour {
    private:
        float speed = 0.03;
    public:
        //Rotate();
        Rotate(float);
        void update(Emitter* emitter) override;
    };

    class MoveByCircle : public Behaviour {
    private:
        eVector2f center;
        float radius;
        float angle;
        float speed;
    public:
        MoveByCircle(eVector2f, float radius, float speed);
        void update(Emitter* emitter) override;
    };

    class Flicker : public Behaviour {
    private:
        float speed;
        bool isActive = true;
        sf::Clock timer;
    public:
        Flicker(float speed);
        void update(Emitter* emitter) override;
    };

    class BindToPlayer : public Behaviour {
    private:
        Player* player;
    public:
        BindToPlayer(Player* player);
        void update(Emitter* emitter) override;
    };


}

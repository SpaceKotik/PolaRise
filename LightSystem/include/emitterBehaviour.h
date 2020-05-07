#pragma once

#include <SFML/Graphics.hpp>
#include "util/SmartVector2/SmartVector2f.h"
class Emitter;
class Player;
class Level;

namespace EmitterBehaviour {

    class Behaviour {
    public:
        Behaviour();
        virtual ~Behaviour() = 0;
        virtual void update(Emitter* emitter){};
        virtual bool mustBeDeleted() {
            return false;
        }

    };

    class MoveByPath : public Behaviour {
    private:
        eVector2f pos1 = {100, 100};
        eVector2f pos2 = {100, 200};
        eVector2f dir;
        float speed = 1;

    public:
        MoveByPath();
        MoveByPath(eVector2f pos1, eVector2f pos2, float speed);
        void update(Emitter*) override;
    };

    class Rotate : public Behaviour {
    private:
        float speed = 0.03;
    public:
        explicit Rotate(float speed);
        void update(Emitter* emitter) override;
    };

    class MoveByCircle : public Behaviour {
    private:
        eVector2f center;
        float radius;
        float angle;
        float speed;
    public:
        MoveByCircle(eVector2f center, float radius, float speed);
        void update(Emitter* emitter) override;
    };

    class Flicker : public Behaviour {
    private:
        float speed;
        bool isActive = true;
        sf::Clock timer;
    public:
        explicit Flicker(float period);
        void update(Emitter* emitter) override;
    };

    class BindToPlayer : public Behaviour {
    private:
        Player* player;
    public:
        BindToPlayer(Player* player);
        void update(Emitter* emitter) override;
    };

    class Shootable : public Behaviour {
    private:
        eVector2f speed;
        eVector2f position;
        Player* player;
        Level* level;
        sf::Clock timer;
        float timeUntilDisappears;
    public:
        Shootable(Player* player, Level* level);
        void update(Emitter* emitter) override;
        bool mustBeDeleted() override;
    };
}

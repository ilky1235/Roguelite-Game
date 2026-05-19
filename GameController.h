#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "player.h"
#include "dynamic_array.h"
#include "enemy.h"
#include "projectile.h"

class GameController {
    public:
        Player player{640, 360};
        dynamic_array<Enemy> enemy_arr{};
        dynamic_array<Projectile> projectiles{};

        GameController();
        void run(double dt);
};

#endif
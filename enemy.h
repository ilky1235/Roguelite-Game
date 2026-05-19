#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "player.h"
#include "projectile.h"

class Enemy : public Entity {
    protected:
        int id{};
        double attack_range{};
        double damage_output{};
        double reload_timer{};

    public:
        Enemy(double x, double y, int assigned_id);
        void draw();
        void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles);
        int get_id() const;
        
};

#endif
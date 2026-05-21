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
        vector_2d velocity{};
        double stun_timer{};
        double flash_timer{};
        double windup_timer{};
        bool is_winding_up{};
        bool is_melee{};

    public:
        Enemy(double x, double y, int assigned_id, bool melee_type);
        void draw();
        void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<rectangle> &pillars);
        int get_id() const;
        void apply_knockback(vector_2d force, double duration);
        
};

#endif
#ifndef ENTITY_H
#define ENTITY_H
#include "splashkit.h"
#include "dynamic_array.h"

class Entity {
    protected:
        point_2d coordinates{};
        double health{};
        double movement_speed{};

    public:
        Entity(double x, double y, double health, double speed);
        void draw();
        void update();
        void take_damage(double amount);
        point_2d get_coordinates() const;
};

class Player : public Entity {
    protected: 
        double dash_cooldown;
        bool is_dashing;
        timer dash_timer;
        bool is_swinging;
        vector_2d swing_dir;
        double swing_timer;
        double sword_cooldown;
        point_2d hitbox{};

    public:
        Player(double x, double y);
        void handle_input(double dt);
        void dash();
        void attack(point_2d target);
        void draw_hitbox();
        point_2d get_attack_hitbox() const;
        bool get_is_swinging() const;
};

class Projectile : public Entity {
    protected:
        double vel_x{};
        double vel_y{};
    public:
        Projectile(point_2d start, point_2d target);

        void update(double dt);
        void draw();
};

class Enemy : public Entity {
    protected:
        double attack_range{};
        double damage_output{};
        double reload_timer{};

    public:
        Enemy(double x, double y);
        void draw();
        void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles);
};





#endif
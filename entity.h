#ifndef ENTITY_H
#define ENTITY_H
#include "splashkit.h"
#include "dynamic_array.h"

class Entity {
    public:
        point_2d coordinates{};
        double health{};
        double movement_speed{};

        Entity(double x, double y, double health, double speed);
        void draw();
        void update();
        void take_damage(double amount);
};

class Player : public Entity {
    public: 
        double dash_cooldown;
        bool is_dashing;
        timer dash_timer;

        Player(double x, double y);
        void handle_input(double dt);
        void dash();
        void attack();
};

class Enemy : public Entity {
    public:
        double attack_range{};
        double damage_output{};
        double reload_timer{};

        Enemy(double x, double y);
        void draw();
        void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles);
};

class Projectile : public Entity {
    public:
        double vel_x{};
        double vel_y{};

        Projectile(point_2d start, point_2d target);

        void update(double dt);
        void draw();
};




#endif
#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"

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
        dynamic_array<int> hit_list;

    public:
        Player(double x, double y);
        void handle_input(double dt);
        void dash();
        void attack(point_2d target);
        void draw_hitbox();
        point_2d get_attack_hitbox() const;
        bool get_is_swinging() const;
        bool has_hit(int enemy_id);
        void add_to_hitlist(int enemy_id);
};


#endif
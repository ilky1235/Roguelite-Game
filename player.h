#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"

enum WeaponType {
    SWORD,
    BOW
};

enum FloorType {
    FLOOR_GRASS,
    FLOOR_STONE
};

enum BoonID {
    BOON_ARES_STRIKE,
    BOON_ARES_SWIFTNESS,

    BOON_ZEUS_STRIKE,
    BOON_ZEUS_SWIFTNESS,
    BOON_ZEUS_DASH
};

struct Boon {
    BoonID id;
    string name;
    string description;
    color theme;
};

class Player : public Entity {
    protected: 
        double dash_cooldown{};
        bool is_dashing{};
        double dash_active_timer{};
        bool is_swinging{};
        vector_2d swing_dir{};
        double swing_timer{};
        double sword_cooldown{};
        double i_frame_timer{};
        point_2d hitbox{};
        dynamic_array<int> hit_list{};
        vector_2d knockback_vec{};
        double knockback_timer{};
        double sword_damage{25.0};
        vector_2d current_velocity{};
        bool is_parrying{};
        double parry_timer{};
        double parry_cooldown{};
        animation player_animation{};
        animation_script player_anim_script{};
        bool is_facing_left{};
        string current_animation{};
        WeaponType weapon = SWORD;
        bool arrow_ready{};
        point_2d arrow_target{};
        dynamic_array<Boon> active_boons{};
        double step_timer;
        FloorType current_floor;
        bool parry_landed = false;
        int melee_combo_step{};
        double combo_window_timer{};
        double combo_damage_mult{1.0};
        bool just_dashed{};

    public:
        Player(double x, double y);
        void handle_input(double dt);
        void dash();
        void draw();
        void attack(point_2d target);
        void draw_hitbox();
        point_2d get_attack_hitbox() const;
        bool get_is_swinging() const;
        bool has_hit(int enemy_id);
        void add_to_hitlist(int enemy_id);
        bool is_dead() const;
        bool is_invincible() const;
        void take_damage(double amount);
        void apply_knockback(vector_2d force, double time);
        void set_coordinates(double new_x, double new_y);
        void increase_damage(double amount);
        void increase_speed(double amount);
        void heal(double amount);
        double get_sword_damage();
        void reset_stats();
        vector_2d get_curr_velocity() const;
        void attempt_parry();
        bool get_is_parrying() const;
        bool is_hitbox_active() const;
        void equip_weapon(WeaponType w);
        WeaponType get_weapon() const;
        bool pull_trigger();
        point_2d get_arrow_target() const;
        void draw_hud();
        void add_boon(Boon b);
        bool has_boon(BoonID target_id);
        void set_floor_type(FloorType f);
        bool pull_trigger(point_2d &target_out);
        bool get_parry_landed() const {return parry_landed;}
        void set_parry_landed(bool val) {parry_landed = val;}
        void grant_iframes(double duration);
        void reset_parry_cooldown();
        void extend_parry();
        double get_combo_mult() const;
        bool get_and_clear_dash_event();
};


#endif
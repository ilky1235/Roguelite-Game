#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "player.h"
#include "projectile.h"

enum BossState {
    B_INTRO,
    B_WALKING,
    B_TRIPLE_BURST,
    B_SHOTGUN,
    B_LUNGE,
    B_MELEE_COMBO,
    B_GROUND_SLAM,
    B_PHASE_TRANSITION,
    B_SHADOW_STEP
};

struct Particle {
    point_2d position{};
    vector_2d velocity{};
    double lifetime{};
    double max_lifetime{};
    color base_color;
    double start_size = 8.0;
};

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
        double ultimate_cooldown{};

    public:
        Enemy(double x, double y, int assigned_id, double health, double speed);
        virtual ~Enemy();
        virtual void draw(bool show_debug);
        virtual void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles);
        int get_id() const;
        virtual void apply_knockback(vector_2d force, double duration);
        virtual bool has_exploded() const { return false; }
        bool is_stunned() const {return stun_timer > 0;}
        virtual bool check_and_clear_aoe(double &out_radius) {return false;}
        virtual double get_collision_radius() const;
        virtual double get_damage_radius() const;
        virtual bool get_is_enraged() const  {return false;}
    
};

class ShooterEnemy : public Enemy {
    private:
        bool is_facing_left = false;
    public:
        ShooterEnemy(double x, double y, int assigned_id);
        void draw(bool show_debug);
        void update(const Player &target, double dt, dynamic_array <Projectile> &projectiles, dynamic_array<Particle> &particles);
        animation enemy_anim;
        string current_anim_name = "Idle";
};

class ChaserEnemy: public Enemy {
    private:
        animation chaser_anim;
        string current_anim_name;
        bool is_facing_left;
    public:
        ChaserEnemy(double x, double y, int assigned_id);
        void draw(bool show_debug);
        void update(const Player &target, double dt, dynamic_array <Projectile> &projectiles, dynamic_array<Particle> &particles);
};

class BossEnemy: public Enemy {
    private:
        BossState current_state{};
        double state_timer{};
        double action_timer{};
        int burst_counter{};
        point_2d locked_target{};
        bool is_enraged = false;
        bool trigger_aoe = false;
        animation boss_anim;
        string current_anim_name;
        bool is_facing_left = false;
        double display_health{};

    public:
        BossEnemy(double x, double y, int assigned_id);
        void update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles);
        void draw(bool show_debug);
        void apply_knockback(vector_2d force, double duration);
        bool check_and_clear_aoe(double &out_radius);
        double get_collision_radius() const;
        double get_damage_radius() const;
        bool get_is_enraged() const {return is_enraged;}
};

class KamikazeEnemy: public Enemy {
    protected:
        double fuse_timer = 4.0;
        bool is_exploded = false;
        bool is_triggered{};

    public:
        KamikazeEnemy(double x, double y, int assigned_id);
        void draw(bool show_debug);
        void update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles);
        bool has_exploded() const;
};

#endif
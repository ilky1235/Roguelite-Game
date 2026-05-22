#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "player.h"
#include "projectile.h"

enum BossState {
    B_WALKING,
    B_TRIPLE_BURST,
    B_SHOTGUN,
    B_LUNGE
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

    public:
        Enemy(double x, double y, int assigned_id, double health, double speed);
        virtual ~Enemy();
        virtual void draw();
        virtual void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles);
        int get_id() const;
        virtual void apply_knockback(vector_2d force, double duration);
        virtual bool has_exploded() const { return false; }
        bool is_stunned() const {return stun_timer > 0;}
        
};

class ShooterEnemy : public Enemy {
    public:
        ShooterEnemy(double x, double y, int assigned_id);
        void draw();
        void update(const Player &target, double dt, dynamic_array <Projectile> &projectiles);
};

class ChaserEnemy: public Enemy {
    public:
        ChaserEnemy(double x, double y, int assigned_id);
        void draw();
        void update(const Player &target, double dt, dynamic_array <Projectile> &projectiles);
};

class BossEnemy: public Enemy {
    private:
        BossState current_state{};
        double state_timer{};
        double action_timer{};
        int burst_counter{};
        point_2d locked_target{};

    public:
        BossEnemy(double x, double y, int assigned_id);
        void update(const Player &target, double dt, dynamic_array<Projectile> &projectiles);
        void draw();
        void apply_knockback(vector_2d force, double duration);
};

class KamikazeEnemy: public Enemy {
    protected:
        double fuse_timer = 4.0;
        bool is_exploded = false;
        bool is_triggered{};

    public:
        KamikazeEnemy(double x, double y, int assigned_id);
        void draw();
        void update(const Player &target, double dt, dynamic_array<Projectile> &projectiles);
        bool has_exploded() const;
};

#endif
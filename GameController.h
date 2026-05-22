#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "player.h"
#include "dynamic_array.h"
#include "enemy.h"
#include "projectile.h"
#include "level_manager.h"

enum game_state {
    PLAYING,
    DEAD,
    VICTORY,
    HUB
};

struct DangerZone {
    point_2d position;
    double radius;
    double timer;
};

struct Particle {
    point_2d position{};
    vector_2d velocity{};
    double lifetime{};
    double max_lifetime{};
};

// NOTE: PhysicalDoor struct has been completely REMOVED from here.
// It now lives inside LevelManager.h!

class GameController {
    private:
        Player player{640, 360};
        dynamic_array<Enemy*> enemy_arr{};
        dynamic_array<Projectile> projectiles{};
        dynamic_array<Particle> particles;
        dynamic_array<DangerZone> danger_zones{};
        
        game_state state{};
        double hit_pause_timer{};
        double screen_shake_timer{};

        // NOTE: ALL root_node, current_room, all_nodes, and active_doors 
        // have been REMOVED! 
        
        // --- NEW ARCHITECTURE ---
        LevelManager level_manager; 

    public:
        GameController();
        ~GameController();
        
        void run(double dt);
        void update_hub(double dt);
        void update_playing(double dt);
        
        void draw_death_screen();
        void draw_victory_screen();
        void reset();
        
        string reward_to_string(reward_type r);
        void apply_reward(reward_type reward);
        
        // NOTE: spawn_wave, generate_graph, and get_random_reward 
        // have been completely REMOVED from here!
};

#endif
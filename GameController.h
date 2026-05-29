#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "player.h"
#include "dynamic_array.h"
#include "enemy.h"
#include "projectile.h"
#include "level_manager.h"
#include <fstream>

enum MusicTrack {
    TRACK_NONE,
    TRACK_HUB,
    TRACK_BATTLE,
    TRACK_BOSS,
    TRACK_BOSS_2,
    TRACK_DEFEAT,
    TRACK_VICTORY
};

enum game_state {
    PLAYING,
    DEAD,
    VICTORY,
    DRAFTING,
    HUB,
    MAIN_MENU
};

struct RewardDrop {
    bool is_active{};
    point_2d position;
    reward_type type;
};

struct DangerZone {
    point_2d position;
    double radius;
    double timer;
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
        MusicTrack current_track;
        
        game_state state{};
        double hit_pause_timer{};
        double screen_shake_timer{};
        // --- NEW ARCHITECTURE ---
        LevelManager level_manager; 
        bool show_debug = false;
        RewardDrop current_reward{};
        bool room_cleared = false;
        dynamic_array<Boon> current_draft_options;
        bool is_paused;
        double hub_float_angle{};
        double arena_radius = 1200;
        point_2d arena_center{};
        animation fire_wall_anim{};
        double death_timer{};
        int current_score = 0;
        int combo_multiplier = 1;
        double combo_timer = 0;
        int top_scores[5] = {0};
        bool is_viewing_leaderboard = false;
        double run_timer{};
        bool time_bonus_applied = false;
        double victory_timer{};
        bool score_sound_played{};

    public:
        GameController();
        ~GameController();
        
        void run(double dt);
        void update_hub(double dt);
        void update_playing(double dt);
        
        void draw_death_screen(double dt);
        void draw_victory_screen(double dt);
        void reset();
        
        string reward_to_string(reward_type r);
        void apply_reward(reward_type reward);
        void update_drafting();
        void populate_draft_menu(reward_type god_type);
        void update_main_menu();
        void draw_pause_menu();
        void load_leaderboard();
        void save_new_score(int final_score);
};

#endif
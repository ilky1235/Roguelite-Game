#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "player.h"
#include "dynamic_array.h"
#include "enemy.h"
#include "projectile.h"

enum game_state {
    PLAYING,
    DEAD
};

class GameController {
    private:
        Player player{640, 360};
        dynamic_array<Enemy> enemy_arr{};
        dynamic_array<Projectile> projectiles{};
        dynamic_array<rectangle> pillars;
        game_state state{};
        double hit_pause_timer{};
        double screen_shake_timer{};

        int current_wave{};
        double wave_transition_timer{};

    public:
        GameController();
        void run(double dt);
        void update_playing(double dt);
        void draw_death_screen();
        void spawn_wave(int wave_number);
        void reset();
};

#endif
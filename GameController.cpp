#include "GameController.h"



GameController::GameController() 
{
    state = PLAYING;
    load_font("TitleFont", "OptimusPrinceps.ttf");
    // Top Left
    pillars.add(rectangle_from(250, 150, 150, 100)); 
    // Top Right
    pillars.add(rectangle_from(880, 150, 150, 100)); 
    // Bottom Left
    pillars.add(rectangle_from(250, 470, 150, 100)); 
    // Bottom Right
    pillars.add(rectangle_from(880, 470, 150, 100));
    current_wave = 1;
    spawn_wave(current_wave);
}

void GameController::run(double dt)
{
    if (state == PLAYING) {
        update_playing(dt);
    }

    else if (state == DEAD) {
        draw_death_screen();
    }
}
void GameController::update_playing(double dt)
{
    double real_dt = dt;

    if (hit_pause_timer > 0) {
        hit_pause_timer -= real_dt;
        dt = 0;
    }

    if (screen_shake_timer > 0) {
        screen_shake_timer -= real_dt;
        move_camera_to(rnd(10) - 5, rnd(10) - 5);

    }

    else {
        move_camera_to(0, 0);
    }

    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt, pillars);

    if (player.get_is_swinging()) {
        point_2d sword_center = player.get_attack_hitbox();

        for (int i = enemy_arr.length() - 1; i >= 0; --i) {
             if (point_point_distance(enemy_arr[i].get_coordinates(), sword_center) <= 65) {
                if (!player.has_hit(enemy_arr[i].get_id())) {
                    enemy_arr[i].take_damage(25);
                    player.add_to_hitlist(enemy_arr[i].get_id());
                    
                    vector_2d opposite_vec = vector_point_to_point(player.get_coordinates(), enemy_arr[i].get_coordinates());

                    if (vector_magnitude(opposite_vec) > 0) {
                        vector_2d knockback = unit_vector(opposite_vec);
                        knockback.x *= 700;
                        knockback.y *= 700;

                        enemy_arr[i].apply_knockback(knockback, 0.4);
                    }
                    hit_pause_timer = 0.03;
                    screen_shake_timer = 0.1;

                }

                if (enemy_arr[i].get_health() <= 0) {
                    enemy_arr.remove(i);

                    if (enemy_arr.length() == 0) {
                        wave_transition_timer = 2.0;
                    }
                    continue;
                 }
             }
            }
    }

    for (int i = projectiles.length() - 1; i >= 0; --i) {
        point_2d pos = projectiles[i].get_coordinates();
        rectangle proj_box = rectangle_from(pos.x, pos.y, 10, 10);
        bool hit_wall = false;

        for (int i = 0; i < pillars.length(); i++) {
            if (rectangles_intersect(proj_box, pillars[i])) {
                hit_wall = true;
                break;
            }
            
        }

        if (hit_wall) {
            projectiles.remove(i);
        }

        if (point_point_distance(pos, player.get_coordinates()) <= 50) {
            if (!player.is_invincible()) {
                player.take_damage(20);
                projectiles.remove(i);
                continue;
            }

        }

        if (pos.x < 0 || pos.x > screen_width() || pos.y < 0 || pos.y > screen_height()) {
            projectiles.remove(i);
            continue;
        }
    }

    for (int i = enemy_arr.length() - 1; i >= 0; i--) {
        enemy_arr[i].update(player, dt, projectiles, pillars);
        enemy_arr[i].draw();

        if (point_point_distance(enemy_arr[i].get_coordinates(), player.get_coordinates()) <= 35) {
            if (!player.is_invincible()) {
                player.take_damage(20);

                screen_shake_timer = 0.2;
                hit_pause_timer = 0.05;
            }
        }
    }
        
        
    for (int i = 0; i < pillars.length(); i++) {
        fill_rectangle(rgba_color(50, 50, 50, 255), pillars[i]);
    }
    for (int i = projectiles.length() - 1 ; i >= 0; i--) {
        projectiles[i].update(dt);
        projectiles[i].draw();
    }

    player.draw();
    player.draw_hitbox();

    if (enemy_arr.length() == 0) {
        if (wave_transition_timer > 0) {
            wave_transition_timer -= dt;

            fill_rectangle(rgba_color(0, 0, 0, 100), 0, 0, screen_width(), screen_height());
            draw_text("WAVE CLEAR", COLOR_WHITE, "Arial", 60, screen_width() / 2 - 170, screen_height() / 2 - 50);
            string next_wave_text = "WAVE " + to_string(current_wave + 1) + " INCOMING...";
            draw_text(next_wave_text, COLOR_RED, "Arial", 30, screen_width() / 2 - 160, screen_height() / 2 + 20);

        } else {
            ++current_wave;
            spawn_wave(current_wave);
        }
    } 
    

    if (player.is_dead()) {
        state = DEAD;
    }

}

void GameController::draw_death_screen()
{
    // dim the screen
    fill_rectangle(rgba_color(0, 0, 0, 180), 0, 0, 1280, 720);
    draw_text("YOU DIED", COLOR_RED, "TitleFont", 150, screen_width() / 2 - 400, screen_height() / 2 - 100);

    draw_text("Press 'R' to Restart", COLOR_WHITE, "Arial", 30, screen_width() / 2 - 120, screen_height() / 2 + 50);

    if (key_typed(R_KEY)) {
        reset();
    }
}


void GameController::spawn_wave(int wave_number)
{
    int num_enemies = wave_number + 2;

    for (int i = 0; i < num_enemies; i++) {
        point_2d spawn{};

        if (rnd() < 0.5) {
            if (rnd() < 0.5) {
                spawn.x = -40;
            }

            else {
                spawn.x = screen_width() + 40;
            }

            spawn.y = rnd(screen_height());
        }

        else {
            if (rnd() < 0.5) {
                spawn.y = -40;
            }

            else {
                spawn.y = screen_height() + 40;
            }
            spawn.x = rnd(screen_width());

        }

        bool spawn_melee = rnd() < 0.4;
        enemy_arr.add(Enemy(spawn.x, spawn.y, i, spawn_melee));
    }


}

void GameController::reset()
{
    enemy_arr.clear();
    projectiles.clear();

    player = Player(640, 360);

    // 3. Reset the Timers & State
    state = PLAYING;
    current_wave = 1;
    wave_transition_timer = 0;
    hit_pause_timer = 0;
    screen_shake_timer = 0;

    // 4. Start the first wave!
    spawn_wave(current_wave);
}
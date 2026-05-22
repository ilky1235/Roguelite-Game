#include "GameController.h"

GameController::GameController() 
{
    state = HUB; // Boot directly into the safe zone
    load_font("TitleFont", "OptimusPrinceps.ttf");
    load_font("TextFont", "CaesarDressing-Regular.ttf");
}

GameController::~GameController()
{
    // Only clean up the enemies. 
    // The LevelManager now handles deleting the graph nodes!
    for (int i = 0; i< enemy_arr.length(); i++) {
        delete enemy_arr[i];
    }
}

void GameController::run(double dt)
{
    if (state == HUB) {
        update_hub(dt);
    }
    else if (state == PLAYING) {
        update_playing(dt);
    }
    else if (state == DEAD) {
        draw_death_screen();
    }
    else if(state == VICTORY) {
        draw_victory_screen();
    }
}

void GameController::update_hub(double dt)
{
    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt);

    draw_text("THE HUB", COLOR_WHITE, "TitleFont", 60, screen_width() / 2 - 120, 50);
    draw_text("Safe Zone", COLOR_LIGHT_GRAY, "TextFont", 20, screen_width() / 2 - 40, 120);

    // The Door to Tartarus
    rectangle tartarus_door = rectangle_from(screen_width() / 2 - 60, 180, 120, 80);
    fill_rectangle(COLOR_PURPLE, tartarus_door);
    draw_text("ENTER", COLOR_WHITE, "TitleFont", 20, tartarus_door.x + 25, tartarus_door.y + 30);

    player.draw();
    player.draw_hitbox();

    rectangle player_hitbox = rectangle_from(player.get_coordinates().x - 20, player.get_coordinates().y - 20, 40, 40);
    
    if (rectangles_intersect(player_hitbox, tartarus_door)) {
        // Delegate graph generation and enemy spawning to the LevelManager
        level_manager.generate_graph();
        level_manager.spawn_wave(enemy_arr, screen_width(), screen_height());
        
        player.set_coordinates(100, screen_height() / 2); 
        state = PLAYING;
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
    } else {
        move_camera_to(0, 0);
    }

    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt);

    // ==========================================
    // --- DEVELOPER CHEATS ---
    // ==========================================
    if (key_typed(K_KEY)) {
        for (int i = 0; i < enemy_arr.length(); i++) {
            delete enemy_arr[i];
        }
        enemy_arr.clear();
    }
    if (key_typed(G_KEY)) {
        player.heal(99999);
    }
    if (key_typed(H_KEY)) {
        player.increase_damage(5000);
    }

    // --- SWORD LOGIC ---
    if (player.get_is_swinging()) {
        point_2d sword_center = player.get_attack_hitbox();

        for (int i = enemy_arr.length() - 1; i >= 0; --i) {
             if (point_point_distance(enemy_arr[i]->get_coordinates(), sword_center) <= 65) {
                if (!player.has_hit(enemy_arr[i]->get_id())) {
                    enemy_arr[i]->take_damage(player.get_sword_damage());
                    player.add_to_hitlist(enemy_arr[i]->get_id());
                    
                    vector_2d opposite_vec = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());

                    if (vector_magnitude(opposite_vec) > 0) {
                        vector_2d knockback = unit_vector(opposite_vec);
                        knockback.x *= 700;
                        knockback.y *= 700;

                        enemy_arr[i]->apply_knockback(knockback, 0.4);
                    }

                    double baseline_hitstop = 0.05;
                    if (hit_pause_timer < baseline_hitstop) {
                        hit_pause_timer = baseline_hitstop;
                    }
                    screen_shake_timer = 0.1;
                }    
            }
        }
    }

    // --- PROJECTILE LOGIC ---
    for (int i = projectiles.length() - 1; i >= 0; --i) {
        point_2d pos = projectiles[i].get_coordinates();


        if (!projectiles[i].get_is_deflected() && point_point_distance(pos, player.get_coordinates()) <= 50) {

            if (player.get_is_parrying()) {
                hit_pause_timer = 0.1;
                screen_shake_timer = 0.15;

                projectiles[i].deflect();
                continue;
            }

            else if (!player.is_invincible()) {
                player.take_damage(20);
            }
            projectiles.remove(i);
            continue;
        }

        if (pos.x < 0 || pos.x > screen_width() || pos.y < 0 || pos.y > screen_height()) {
            projectiles.remove(i);
            continue;
        }
    }

    // --- ENEMY LOOP & PARTICLES ---
    for (int i = enemy_arr.length() - 1; i >= 0; i--) {
        if (enemy_arr[i]->get_health() <= 0) {

            // Kamikaze

            if (enemy_arr[i]->has_exploded()) {
                DangerZone dz;
                dz.position = enemy_arr[i]->get_coordinates();
                dz.radius = 150;
                dz.timer = 0;
                danger_zones.add(dz);
            }

            for (int p = 0; p < 8; ++p) {
                Particle debris;
                debris.position = enemy_arr[i]->get_coordinates();
                debris.velocity.x = (rnd() * 800) - 400;
                debris.velocity.y = (rnd() * 800) - 400;
                debris.max_lifetime = 0.2 + (rnd() * 0.3);
                debris.lifetime = debris.max_lifetime;
                particles.add(debris);
            }

            delete enemy_arr[i];
            enemy_arr.remove(i);
            continue;
        }
        enemy_arr[i]->update(player, dt, projectiles);
        enemy_arr[i]->draw();

        // ENEMY VS DEFLECTED PROJECTILE COLLISION
        for (int j = projectiles.length() - 1; j >= 0; --j) {
            if (projectiles[j].get_is_deflected()) {
                if (point_point_distance(enemy_arr[i]->get_coordinates(), projectiles[j].get_coordinates()) <= 40) {
                    enemy_arr[i]->take_damage(70);

                    vector_2d kb = vector_point_to_point(projectiles[j].get_coordinates(), enemy_arr[i]->get_coordinates());

                    if (vector_magnitude(kb) > 0) {
                        kb = unit_vector(kb);
                        kb.x *= 600;
                        kb.y *= 600;
                        enemy_arr[i]->apply_knockback(kb, 0.4);
                    }   
                    projectiles.remove(j);
                }
            }
        }

        // MELEE COLLISION
        if (enemy_arr[i]->is_stunned()) continue;

        if (point_point_distance(enemy_arr[i]->get_coordinates(), player.get_coordinates()) <= 35) {

            // Sekiro Parry
            if (player.get_is_parrying()) {
                hit_pause_timer = 0.15;
                screen_shake_timer = 0.2;

                vector_2d parry_force = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());
                if (vector_magnitude(parry_force) > 0) {
                    parry_force = unit_vector(parry_force);
                    parry_force.x *= 1200; // Violent knockback
                    parry_force.y *= 1200;
                    
                    // Stun them for a massive 1.2 seconds so you can counter-attack
                    enemy_arr[i]->apply_knockback(parry_force, 1.2); 
                }
            }
            else if (!player.is_invincible()) {
                player.take_damage(20);
                screen_shake_timer = 0.2; 
                hit_pause_timer = 0.05;   

                vector_2d knockback = vector_point_to_point(enemy_arr[i]->get_coordinates(), player.get_coordinates());

                if (vector_magnitude(knockback) > 0) {
                    vector_2d kb_dir = unit_vector(knockback);
                    kb_dir.x *= 900;
                    kb_dir.y *= 900;
                    player.apply_knockback(kb_dir, 0.4);
                }

                vector_2d raw_enemy_kb = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());

                if (vector_magnitude(raw_enemy_kb) > 0) {
                    vector_2d enemy_kb_dir = unit_vector(raw_enemy_kb);
                    enemy_kb_dir.x *= 300;
                    enemy_kb_dir.y *= 300;
                    enemy_arr[i]->apply_knockback(enemy_kb_dir, 0.6);
                }
            }
        }
    }
        
    for (int i = projectiles.length() - 1 ; i >= 0; i--) {
        projectiles[i].update(dt);
        projectiles[i].draw();
    }

    // --- PARTICLE PHYSICS & RENDER LOOP ---
    for (int i = particles.length() - 1; i >= 0; --i) {
        particles[i].position.x += particles[i].velocity.x * dt;
        particles[i].position.y += particles[i].velocity.y * dt;

        particles[i].velocity.x -= particles[i].velocity.x * 12.0 * dt;
        particles[i].velocity.y -= particles[i].velocity.y * 12.0 * dt;

        particles[i].lifetime -= dt;
        if (particles[i].lifetime <= 0) {
            particles.remove(i);
        } else {
            double size_ratio = particles[i].lifetime / particles[i].max_lifetime;
            int size = 8 * size_ratio; 
            
            color p_color = rgba_color(255.0, 50 * (205 * size_ratio), 0.0, 255 * size_ratio); 
            fill_rectangle(p_color, particles[i].position.x, particles[i].position.y, size, size);
        }
    }

    player.draw();
    player.draw_hitbox();

    // ==========================================
    // --- THE LEVEL MANAGER INTEGRATION ---
    // ==========================================
    if (enemy_arr.length() == 0) {
        if (level_manager.get_current_room()->get_reward() == BOSS) {
            state = VICTORY;
            return;
        }

        if (level_manager.get_active_doors().length() == 0) {
            projectiles.clear();
            level_manager.generate_doors(screen_width(), screen_height());
        }
    }

    dynamic_array<PhysicalDoor>& active_doors = level_manager.get_active_doors();
    
    for (int i = 0; i < active_doors.length(); i++) {
        fill_rectangle(COLOR_BLACK, active_doors[i].bounds);
        
        string reward_text = reward_to_string(active_doors[i].destination->get_reward());
        draw_text(reward_text, COLOR_BLACK, "TextFont", 16, active_doors[i].bounds.x - 40, active_doors[i].bounds.y - 20);

        rectangle player_hitbox = rectangle_from(player.get_coordinates().x - 20, player.get_coordinates().y - 20, 40, 40);
        
        if (rectangles_intersect(player_hitbox, active_doors[i].bounds)) {
            apply_reward(active_doors[i].destination->get_reward());
            
            level_manager.set_current_room(active_doors[i].destination);
            level_manager.clear_doors();
            level_manager.spawn_wave(enemy_arr, screen_width(), screen_height());
            
            player.set_coordinates(100, screen_height() / 2);
            break; 
        }
    }

    // ==========================================
    // --- THE DANGER ZONE (DETONATION) LOOP ---
    // ==========================================
    for (int i = danger_zones.length() - 1; i >= 0; --i) {
        danger_zones[i].timer -= dt;

        // Since the Kamikaze passes a 0.0 timer, this triggers instantly!
        if (danger_zones[i].timer <= 0) {
            
            screen_shake_timer = 0.3; // Violent screen shake
            hit_pause_timer = 0.1;
            
            // Did the player get caught inside the 150px radius?
            if (point_point_distance(player.get_coordinates(), danger_zones[i].position) <= danger_zones[i].radius) {
                if (!player.is_invincible()) {
                    player.take_damage(40); 
                    
                    // Violent knockback out of the explosion center
                    vector_2d kb = vector_point_to_point(danger_zones[i].position, player.get_coordinates());
                    if (vector_magnitude(kb) > 0) {
                        kb = unit_vector(kb);
                        kb.x *= 1500;
                        kb.y *= 1500;
                        player.apply_knockback(kb, 0.5);
                    }
                }
            }
            // Remove the bomb from memory now that it has exploded
            danger_zones.remove(i);
        }
    }
    
    
    if (player.is_dead()) {
        state = DEAD;
    }
}

void GameController::draw_death_screen()
{
    fill_rectangle(rgba_color(0, 0, 0, 180), 0, 0, screen_width(), screen_height());
    draw_text("YOU DIED", COLOR_RED, "TitleFont", 150, screen_width() / 2 - 400, screen_height() / 2 - 100);
    draw_text("Press 'R' to Return to Hub", COLOR_WHITE, "Arial", 30, screen_width() / 2 - 160, screen_height() / 2 + 50);

    if (key_typed(R_KEY)) {
        reset();
    }
}

void GameController::draw_victory_screen()
{
    fill_rectangle(rgba_color(0, 0, 0, 180), 0, 0, screen_width(), screen_height());
    draw_text("VICTORY", COLOR_YELLOW, "TitleFont", 150, screen_width() / 2 - 350, screen_height() / 2 - 100);
    draw_text("The Titan has fallen.", COLOR_WHITE, "TextFont", 40, screen_width() / 2 - 200, screen_height() / 2 + 20);
    draw_text("Press 'R' to Return to Hub", COLOR_LIGHT_GRAY, "Arial", 30, screen_width() / 2 - 160, screen_height() / 2 + 100);

    if (key_typed(R_KEY)) {
        reset();
    }
}

void GameController::reset()
{
    for (int i = 0; i < enemy_arr.length(); i++) {
        delete enemy_arr[i];
    }
    enemy_arr.clear();
    projectiles.clear();
    particles.clear();
    danger_zones.clear();

    player.reset_stats();

    hit_pause_timer = 0;
    screen_shake_timer = 0;

    // Send the player back to the safe zone
    state = HUB;
    player.set_coordinates(screen_width() / 2, screen_height() - 100); 
}

string GameController::reward_to_string(reward_type r) {
    if (r == HEAL)   return "HEAL +100 HP";
    if (r == SPEED)  return "SPEED BOOST";
    if (r == DAMAGE) return "DAMAGE BOOST";
    if (r == BOSS)   return "BOSS ROOM";
    return "???";
}

void GameController::apply_reward(reward_type reward)
{
    if (reward == HEAL) {
        player.heal(50);
    }
    else if (reward == SPEED) {
        player.increase_speed(50);
    }
    else if (reward == DAMAGE) {
        player.increase_damage(50);
    }
}
#include "GameController.h"


GameController::GameController() 
{
    state = MAIN_MENU; // Boot directly into the safe zone
    current_track = TRACK_NONE;
    load_font("TitleFont", "OptimusPrinceps.ttf");
    load_font("TextFont", "CaesarDressing-Regular.ttf");
    load_font("MenuFont", "SpectralSC-SemiBold.ttf");
    load_bitmap("Grass", "grass.png");
    load_bitmap("FloorBase", "stonetile.png");
    load_bitmap("FloorBase2", "stonetile2.png");
    load_sound_effect("Parry", "parry.wav");
    load_sound_effect("Sword", "sword.mp3");
    load_sound_effect("Impact", "impact.wav");
    load_sound_effect("Dash", "dash.mp3");
    load_sound_effect("StepStone", "concretestep.mp3");
    load_sound_effect("StepGrass", "grassstep.mp3");
    load_music("HubMusic", "trees.wav");
    load_music("BattleMusic", "battle.wav");
    load_music("BossMusic", "boss.wav");
    load_music("BossMusic2", "boss2.mp3");
    load_bitmap("SwordSprite", "Sword.png");
    load_bitmap("BowSprite", "Bow.png");
    load_bitmap("ArrowSprite", "Arrow.png");
    load_sound_effect("BossRoar", "bossroar.wav");
    load_sound_effect("BossSwing", "bossattack.mp3");
    load_sound_effect("BossLunge", "bossdash.mp3");
    load_sound_effect("BossSlam", "bossslam.mp3");
    load_sound_effect("Hurt", "hurt.mp3");
    load_music("DefeatMusic", "lose.wav");
    load_sound_effect("Explosion", "explosion.mp3");
    load_sound_effect("Combo", "combo.mp3");
    load_music("VictoryMusic", "win.wav");
    load_sound_effect("Clink", "clink.mp3");
    load_leaderboard();
    arena_center.x = screen_width() / 2.0;
    arena_center.y = screen_height() / 2.0;
    fire_wall_anim = create_animation(animation_script_named("FireAnim"), "Burn");
   

}

GameController::~GameController()
{
    // Only clean up the enemies
    // The LevelManager now handles deleting the graph nodes!
    for (int i = 0; i< enemy_arr.length(); i++) {
        delete enemy_arr[i];
    }
}

void GameController::run(double dt)
{
    if (key_typed(ESCAPE_KEY)) {
        if (state == PLAYING || state == HUB) {
            is_paused = !is_paused;

            if (is_paused) pause_music();
            else resume_music();
        }
    }

    double active_dt = is_paused ? 0.0 : dt;

    if (state == HUB) {
        update_hub(active_dt);
    }
    else if (state == PLAYING) {
        update_playing(active_dt);
    }
    else if (state == DEAD) {
        draw_death_screen(active_dt);
    }
    else if(state == VICTORY) {
        draw_victory_screen(active_dt);
    } else if (state == DRAFTING) {
        update_drafting();
    }else if (state == MAIN_MENU) {
        update_main_menu();
    }

    if (is_paused) {
        draw_pause_menu();
    }
}

void GameController::update_hub(double dt)
{
    if (current_track != TRACK_HUB) {
        play_music(music_named("HubMusic"), -1, 0.6f);
        current_track = TRACK_HUB;
    }

    player.set_floor_type(FLOOR_GRASS);
    // Draw the floor
    for (int x = -32 ; x < screen_width() + 32; x += 32) {
        for (int y = -32; y < screen_height() + 32; y +=32) {
            draw_bitmap(bitmap_named("Grass"), x, y);
        }
    }

    // ==========================================
    // --- THE WEAPON DROP ZONE ---
    // ==========================================

    hub_float_angle += 180 *dt;
    if (hub_float_angle >= 360) hub_float_angle -= 360;

    double y_offset = sine(hub_float_angle) * 8.0;

    point_2d sword_rack_pos = {screen_width() / 2.0 - 400.0, 250.0};
    point_2d bow_rack_pos = { screen_width() / 2.0 - 300.0, 250.0};
    rectangle player_hitbox = rectangle_from(player.get_coordinates().x - 20, player.get_coordinates().y - 20, 40, 40);

    // ==========================================
    // --- THE LEADERBOARD MONUMENT ---
    // ==========================================
    rectangle board_rect = rectangle_from(screen_width() / 2.0 + 300, 250, 80, 120);
    fill_rectangle(rgba_color(30, 30, 35, 255), board_rect); // A dark slate monument
    draw_rectangle(COLOR_GOLD, board_rect);
    draw_text("RECORDS", COLOR_GOLD, "MenuFont", 12, board_rect.x + 15, board_rect.y + 10);

    // Interaction Check
    if (rectangles_intersect(player_hitbox, board_rect)) {
        if (!is_viewing_leaderboard) {
            draw_text("Press 'E' to View Records", COLOR_YELLOW, "TextFont", 16, board_rect.x - 40, board_rect.y - 30);
        }
        
        if (key_typed(E_KEY)) {
            is_viewing_leaderboard = !is_viewing_leaderboard; // Toggle the UI
        }
    } else {
        is_viewing_leaderboard = false; // Close it automatically if they walk away
    }

    

    // ==========================================
    // --- RENDER THE WEAPONS ---
    // ==========================================
    
    // THE SWORD RACK
    if (player.get_weapon() != SWORD) {
        // Draw floating Sword
        draw_bitmap(bitmap_named("SwordSprite"), sword_rack_pos.x - 15, sword_rack_pos.y - 40 + y_offset);
    } else {
        // If holding the sword, show the rack is empty
        draw_text("(Equipped)", COLOR_DARK_GRAY, "MenuFont", 12, sword_rack_pos.x - 25, sword_rack_pos.y - 20);
    }

    // THE BOW RACK
    if (player.get_weapon() != BOW) {
        // Draw floating Bow
        draw_bitmap(bitmap_named("BowSprite"), bow_rack_pos.x - 15, bow_rack_pos.y - 40 + y_offset);
    } else {

        // If holding the bow, show the rack is empty
        draw_text("(Equipped)", COLOR_DARK_GRAY, "MenuFont", 12, bow_rack_pos.x - 25, bow_rack_pos.y - 20);
    }

    // ==========================================
    // --- INTERACTION LOGIC ---
    // ==========================================
    
    rectangle sword_hitbox = rectangle_from(sword_rack_pos.x - 30, sword_rack_pos.y - 30, 60, 60);
    rectangle bow_hitbox = rectangle_from(bow_rack_pos.x - 30, bow_rack_pos.y - 30, 60, 60);

    // Check Sword Pick-up
    if (rectangles_intersect(player_hitbox, sword_hitbox) && player.get_weapon() != SWORD) {
        draw_text("Press 'E' to take Sword", COLOR_YELLOW, "TextFont", 16, sword_rack_pos.x - 60, sword_rack_pos.y - 60);
        
        if (key_typed(E_KEY)) {
            player.equip_weapon(SWORD);
        }
    }

    // Check Bow Pick-up
    if (rectangles_intersect(player_hitbox, bow_hitbox) && player.get_weapon() != BOW) {
        draw_text("Press 'E' to take Bow", COLOR_YELLOW, "TextFont", 16, bow_rack_pos.x - 50, bow_rack_pos.y - 60);
        
        if (key_typed(E_KEY)) {
            player.equip_weapon(BOW);
        }
    }

    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt);

    // ==========================================
    // --- HUB PROJECTILE LOGIC ---
    // ==========================================

    point_2d target_pos;

    if (player.pull_trigger(target_pos)) {
        Projectile new_arrow(player.get_coordinates(), target_pos);
        new_arrow.set_is_friendly(true);
        projectiles.add(new_arrow);
    }

    for (int i = projectiles.length() - 1 ; i>= 0 ; --i) {
        projectiles[i].update(dt);
        projectiles[i].draw();

        point_2d pos = projectiles[i].get_coordinates();
        if (pos.x < 0 || pos.x > screen_width() || pos.y < 0 || pos.y > screen_height()) {
            projectiles.remove(i);
        }
    }
    

    draw_text("THE HUB", COLOR_WHITE, "MenuFont", 60, screen_width() / 2 - 120, 50);
    draw_text("Safe Zone", COLOR_LIGHT_GRAY, "TextFont", 20, screen_width() / 2 - 40, 120);

    // The Door to Tartarus
    rectangle tartarus_door = rectangle_from(screen_width() / 2 - 60, 180, 120, 80);
    fill_rectangle(COLOR_PURPLE, tartarus_door);
    draw_text("ENTER", COLOR_WHITE, "TitleFont", 20, tartarus_door.x + 25, tartarus_door.y + 30);

    player.draw();
    if (show_debug) {
        player.draw_hitbox();
    }

    player.draw_hud();
    
    
    if (rectangles_intersect(player_hitbox, tartarus_door)) {
        // Delegate graph generation and enemy spawning to the LevelManager
        level_manager.generate_graph();
        level_manager.spawn_wave(enemy_arr, screen_width(), screen_height());
        
        player.set_coordinates(100, screen_height() / 2); 
        state = PLAYING;
    }

    // Draw the UI Overlay if active
    if (is_viewing_leaderboard) {
        fill_rectangle(rgba_color(10, 10, 15, 230), 0, 0, screen_width(), screen_height());
        draw_text("H A L L   O F   H E R O E S", COLOR_GOLD, "TitleFont", 60, screen_width() / 2 - 280, 150);
        
        for (int i = 0; i < 5; ++i) {
            string rank = to_string(i + 1) + ".";
            string score_val = to_string(top_scores[i]);
            
            int y_pos = 280 + (i * 60);
            
            // Draw Rank
            draw_text(rank, COLOR_LIGHT_GRAY, "TitleFont", 35, screen_width() / 2 - 150, y_pos);
            // Draw Score
            draw_text(score_val, COLOR_WHITE, "TitleFont", 35, screen_width() / 2 + 50, y_pos);
        }
        
        draw_text("Press 'E' or Walk Away to Close", COLOR_GRAY, "MenuFont", 20, screen_width() / 2 - 140, screen_height() - 100);
    }
}

void GameController::update_playing(double dt)
{

    if (!is_paused) {
        run_timer += dt;
    if (!room_cleared && enemy_arr.length() > 0) {
        if (level_manager.get_current_room()->get_reward() == BOSS) {
            BossEnemy* boss = static_cast<BossEnemy*>(enemy_arr[0]);

            if (boss->get_is_enraged()) {
                if (current_track != TRACK_BOSS_2) {
                    play_music(music_named("BossMusic2"), -1, 0.8f);
                    current_track = TRACK_BOSS_2;
                }
            } else {
                if (current_track != TRACK_BOSS) {
                    play_music(music_named("BossMusic"), -1, 0.8f);
                    current_track = TRACK_BOSS;
                } else {
                    resume_music();
                }
            }
        } else {
            if (current_track != TRACK_BATTLE) {
                play_music(music_named("BattleMusic"), -1, 0.5f);
                current_track = TRACK_BATTLE;
            } else {
                resume_music();
            }
        }
    }
}
    player.set_floor_type(FLOOR_STONE);
    // Draw dungeon floor
    int tile_size = 64;

    for (int x = -tile_size; x < screen_width() + tile_size; x += tile_size) {
        for (int y = -tile_size; y < screen_height() + tile_size; y += tile_size) {
        
                draw_bitmap(bitmap_named("FloorBase"), x , y);
           
        }
    }

    int border_thickness = 60;
    color abyss_color = rgba_color(10, 10, 15, 255); // Deep, dark void
    
    // Draw the 4 massive rectangles blocking out the edges of the room
    fill_rectangle(abyss_color, 0, 0, screen_width(), border_thickness); // Top
    fill_rectangle(abyss_color, 0, screen_height() - border_thickness, screen_width(), border_thickness); // Bottom
    fill_rectangle(abyss_color, 0, 0, border_thickness, screen_height()); // Left
    fill_rectangle(abyss_color, screen_width() - border_thickness, 0, border_thickness, screen_height()); // Right

    // Draw a crisp silver trim around the edge of the floor so it pops!
    draw_rectangle(COLOR_LIGHT_GRAY, border_thickness, border_thickness, screen_width() - (border_thickness * 2), screen_height() - (border_thickness * 2));

    // ==========================================
    // --- THE ARENA CORRUPTION EFFECT ---
    // ==========================================

    bool arena_corrupted = false;

    for (int i = 0; i < enemy_arr.length(); i++) {
        if (enemy_arr[i]->get_is_enraged()) {
            arena_corrupted = true;
            break;
        }
    }

    if (arena_corrupted) {
        // 2. Tick the shared animation forward
        update_animation(fire_wall_anim);

        // 3. Bathe the entire room in a dark, oppressive crimson shadow
        fill_rectangle(rgba_color(40, 0, 10, 150), camera_x(), camera_y(), screen_width(), screen_height());

        // 4. Shrink the arena!
        if (arena_radius > 550.0) {
            arena_radius -= 100.0 * dt; 
        }

        // 5. Draw the Animated Fire Ring
        bitmap fire_bmp = bitmap_named("FireWall");
        double cell_w = bitmap_cell_width(fire_bmp); 
        
        double circumference = 2 * 3.14159 * arena_radius;
        int num_fires = circumference / (cell_w * 0.9); // Overlap slightly to prevent gaps

        drawing_options opts = option_with_animation(fire_wall_anim);

        for (int i = 0; i < num_fires; ++i) {
            double angle = (360.0 / num_fires) * i;
            
            double draw_x = arena_center.x + cosine(angle) * arena_radius - (cell_w / 2.0);
            double draw_y = arena_center.y + sine(angle) * arena_radius - (bitmap_cell_height(fire_bmp) / 2.0);
            
            draw_bitmap(fire_bmp, draw_x, draw_y, opts);
        }

        // 6. The Fire Wall Hitbox (Electric Fence)
        double player_dist = point_point_distance(player.get_coordinates(), arena_center);
        if (player_dist > arena_radius - 20) { 
            
            // Violent bounce back
            vector_2d bounce_dir = vector_point_to_point(player.get_coordinates(), arena_center);
            if (vector_magnitude(bounce_dir) > 0) {
                bounce_dir = unit_vector(bounce_dir);
                bounce_dir.x *= 1200;
                bounce_dir.y *= 1200;
                player.apply_knockback(bounce_dir, 0.4);
            }

        }

        // 7. Ambient Embers
        if (rnd() < 0.3) { 
            Particle ember;
            ember.position.x = camera_x() + rnd(screen_width());
            ember.position.y = camera_y() + screen_height() + 10; 
            ember.velocity.y = -50 - (rnd() * 100); 
            ember.velocity.x = (rnd() * 40) - 20;   
            ember.max_lifetime = 2.0 + rnd() * 2.0; 
            ember.lifetime = ember.max_lifetime;
            ember.base_color = rgba_color(255, 50, 0, 255); 
            ember.start_size = 4 + rnd() * 6;
            particles.add(ember);
        }
    }
    
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

    // ZEUS DASH
    if (player.get_and_clear_dash_event()) {
        if (player.has_boon(BOON_ZEUS_DASH)) {
            for (int p = 0; p < 15; p++) {
                Particle spark;
                spark.base_color = COLOR_YELLOW;
                spark.position = player.get_coordinates();
                double angle = (360.0 / 15) * p;
                double speed = 600 + (rnd() * 300); 
                spark.velocity.x = cosine(angle) * speed;
                spark.velocity.y = sine(angle) * speed;
                spark.max_lifetime = 0.2;
                spark.lifetime = spark.max_lifetime;
                spark.start_size = 10;
                particles.add(spark);
            }

            for (int i = 0; i < enemy_arr.length(); i++) {
                if (point_point_distance(player.get_coordinates(), enemy_arr[i]->get_coordinates()) <= 250) {
                    
                    // Base 30 Lightning Damage
                    enemy_arr[i]->take_damage(30); 
                    
                    // Violent stun & knockback to clear space
                    vector_2d kb = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());
                    if (vector_magnitude(kb) > 0) {
                        kb = unit_vector(kb);
                        kb.x *= 700;
                        kb.y *= 700;
                        enemy_arr[i]->apply_knockback(kb, 0.3);
                    }

                    // Strike particles on the enemy
                    for (int p = 0; p < 5; ++p) {
                        Particle bolt;
                        bolt.base_color = COLOR_WHITE;
                        bolt.position = enemy_arr[i]->get_coordinates();
                        bolt.velocity.x = (rnd() * 1000) - 500;
                        bolt.velocity.y = (rnd() * 1000) - 500;
                        bolt.max_lifetime = 0.15;
                        bolt.lifetime = bolt.max_lifetime;
                        particles.add(bolt);
                    }
            }
        }
    }
}

    // ==========================================
    // --- FIRE BOW LOGIC ---
    // ==========================================
    point_2d target_pos;
    if (player.pull_trigger(target_pos)) {

        // 3. Create the projectile (adjust this line to match your exact constructor!)
        Projectile new_arrow(player.get_coordinates(), target_pos);
        
        // 4. FLAG IT AS FRIENDLY!
        new_arrow.set_is_friendly(true); 

        // 5. Add it to the array
        projectiles.add(new_arrow);
    }
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

    if (key_typed(B_KEY)) {
        // 1. Wipe out any existing enemies in the room
        for (int i = 0; i < enemy_arr.length(); i++) {
            delete enemy_arr[i];
        }
        enemy_arr.clear();
        
        // 2. Spawn the Titan perfectly in the center! (99 is just a placeholder ID)
        enemy_arr.add(new BossEnemy(screen_width() / 2, screen_height() / 2, 99));
    }

    if (key_typed(H_KEY)) {
        player.increase_damage(5000);
    }

    if (key_typed(P_KEY)) {
        show_debug = !show_debug;
    }

    if (combo_timer > 0) {
        combo_timer -= dt;
        if (combo_timer <= 0) {
            combo_multiplier = 1;
        }
    }

    // --- SWORD LOGIC ---
    if (player.is_hitbox_active()) {
        point_2d sword_center = player.get_attack_hitbox();

        for (int i = enemy_arr.length() - 1; i >= 0; --i) {
             if (point_point_distance(enemy_arr[i]->get_coordinates(), sword_center) <= 50 + enemy_arr[i]->get_collision_radius()) {
                if (!player.has_hit(enemy_arr[i]->get_id())) {
                    current_score += (10 * combo_multiplier);
                    combo_multiplier++;
                    if (combo_multiplier > 10) combo_multiplier = 10;
                    combo_timer = 2.5;
                    
                    play_sound_effect("Impact", 1, 0.7);
                    enemy_arr[i]->take_damage(player.get_sword_damage());
                    player.add_to_hitlist(enemy_arr[i]->get_id());

                    // ==========================================
                    // --- NEW: ZEUS CHAIN LIGHTNING ---
                    // ==========================================
                    if (player.has_boon(BOON_ZEUS_STRIKE)) {
                        for (int j = 0; j < enemy_arr.length(); ++j) {
                            if (i != j && point_point_distance(enemy_arr[i]->get_coordinates(), enemy_arr[j]->get_coordinates()) <= 100) {
                                enemy_arr[j]->take_damage(15);

                                for (int p = 0; p < 5; ++p) {
                                    Particle spark;
                                    spark.base_color = COLOR_YELLOW;
                                    spark.position = enemy_arr[j]->get_coordinates();
                                    spark.velocity.x = (rnd() * 1000) - 500;
                                    spark.velocity.y = (rnd() * 1000) - 500;
                                    spark.max_lifetime = 0.15;
                                    spark.lifetime = spark.max_lifetime;
                                    particles.add(spark);
                                }
                            }
                        }
                    }
                    
                    // Check if this is the heavy 3rd strike!
                    bool is_heavy_hit = (player.get_combo_mult() >= 2.0);

                    vector_2d opposite_vec = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());

                    if (vector_magnitude(opposite_vec) > 0) {
                        vector_2d knockback = unit_vector(opposite_vec);
                        
                        // Double the knockback force and stun duration for the heavy finisher!
                        if (is_heavy_hit) {
                            knockback.x *= 1500;
                            knockback.y *= 1500;
                            enemy_arr[i]->apply_knockback(knockback, 0.7);


                            for (int j = 0; j < enemy_arr.length(); ++j) {
                                // If it's a different enemy, and they are within 120 pixels of the impact:
                                if (i != j && point_point_distance(enemy_arr[i]->get_coordinates(), enemy_arr[j]->get_coordinates()) <= 120) {
                                    
                                    // 1. Deal splash damage (half of the heavy attack's damage)
                                    enemy_arr[j]->take_damage(player.get_sword_damage() * 0.5);

                                    // 2. Blast them away from the epicenter of the explosion!
                                    vector_2d splash_kb = vector_point_to_point(enemy_arr[i]->get_coordinates(), enemy_arr[j]->get_coordinates());
                                    if (vector_magnitude(splash_kb) > 0) {
                                        splash_kb = unit_vector(splash_kb);
                                        splash_kb.x *= 900; // Strong radial knockback
                                        splash_kb.y *= 900;
                                        enemy_arr[j]->apply_knockback(splash_kb, 0.5);
                                    }
                                }
                            }
                            for (int p = 0; p < 15; ++p) {
                                Particle blast;
                                blast.base_color = COLOR_WHITE;
                                if (rnd() < 0.5) blast.base_color = COLOR_LIGHT_GRAY;
                                
                                blast.position = enemy_arr[i]->get_coordinates();
                                
                                // Explode outward in a perfect 360 circle
                                double angle = (360.0 / 15) * p;
                                double speed = 400 + (rnd() * 400); 
                                blast.velocity.x = cosine(angle) * speed;
                                blast.velocity.y = sine(angle) * speed;
                                
                                blast.max_lifetime = 0.25;
                                blast.lifetime = blast.max_lifetime;
                                blast.start_size = 10 + rnd(10);
                                particles.add(blast);
                            }
                        } else {
                            knockback.x *= 700;
                            knockback.y *= 700;
                            enemy_arr[i]->apply_knockback(knockback, 0.4);
                        }
                    }

                    // --- DYNAMIC HIT-STOP & SCREEN SHAKE ---
                    // Freeze the game for 0.12s on a heavy hit to simulate the blade "catching"
                    double target_hitstop = is_heavy_hit ? 0.12 : 0.05;
                    if (hit_pause_timer < target_hitstop) {
                        hit_pause_timer = target_hitstop;
                    }
                    
                    // Violent camera vibration
                    double target_shake = is_heavy_hit ? 0.3 : 0.1;
                    if (screen_shake_timer < target_shake) {
                        screen_shake_timer = target_shake;
                    }
                }    
            }
        }
    }

    // --- PROJECTILE LOGIC ---
    for (int i = projectiles.length() - 1; i >= 0; --i) {
        point_2d pos = projectiles[i].get_coordinates();


        if (!projectiles[i].get_is_deflected() && !projectiles[i].get_is_friendly() && point_point_distance(pos, player.get_coordinates()) <= 25) {

            if (player.get_is_parrying()) {
                current_score += (25 * combo_multiplier);
                combo_multiplier++;
                if (combo_multiplier > 10) combo_multiplier = 10;
                combo_timer = 2.5;
                hit_pause_timer = 0.1;
                screen_shake_timer = 0.15;
                play_sound_effect("Parry");

                vector_2d arrow_kb = vector_point_to_point(pos, player.get_coordinates());
                if (vector_magnitude(arrow_kb) > 0) {
                    arrow_kb = unit_vector(arrow_kb);
                    arrow_kb.x *= 400;
                    arrow_kb.y *= 400;

                    player.apply_knockback(arrow_kb, 0.25);
                }

                projectiles[i].deflect();
                continue;
            }

            else if (!player.is_invincible()) {
                combo_multiplier = 1;
                combo_timer = 0;

                player.take_damage(20);
                screen_shake_timer = 0.1;
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
                dz.radius = 100;
                dz.timer = 0;
                danger_zones.add(dz);
            }

            for (int p = 0; p < 8; ++p) {
                Particle debris;
                debris.base_color = COLOR_LIGHT_GRAY;
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
        enemy_arr[i]->update(player, dt, projectiles, particles);
        enemy_arr[i]->draw(show_debug);

        // BOSS AOE
        double aoe_radius = 0;
        if (enemy_arr[i]->check_and_clear_aoe(aoe_radius)) {
            DangerZone dz;
            dz.position = enemy_arr[i]->get_coordinates();
            dz.radius = aoe_radius;
            dz.timer = 0;
            danger_zones.add(dz);
        }

        // ENEMY VS DEFLECTED PROJECTILE COLLISION
        for (int j = projectiles.length() - 1; j >= 0; --j) {
            if (projectiles[j].get_is_deflected() || projectiles[j].get_is_friendly()) {
                if (point_point_distance(enemy_arr[i]->get_coordinates(), projectiles[j].get_coordinates()) <= 10 + enemy_arr[i]->get_collision_radius()) {
                    play_sound_effect("Impact", 1, 0.7);
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
        double dist = point_point_distance(enemy_arr[i]->get_coordinates(), player.get_coordinates());

        double damage_threshold = enemy_arr[i]->get_damage_radius() + 20.0; 

        // 1. CHECK PARRY FIRST (With a 25px forgiveness buffer!)
        if (dist <= damage_threshold + 25.0 && player.get_is_parrying() && !player.get_parry_landed()) {
            
            player.set_parry_landed(true);
            // Grant score
            current_score += (50 * combo_multiplier);
            combo_multiplier++;
            if (combo_multiplier > 10) combo_multiplier = 10;
            combo_timer = 2.5;

            player.grant_iframes(0.7);
            player.reset_parry_cooldown();
         

            hit_pause_timer = 0.15;
            screen_shake_timer = 0.2;
            play_sound_effect("Parry");

            vector_2d player_slide = vector_point_to_point(enemy_arr[i]->get_coordinates(), player.get_coordinates());
            if (vector_magnitude(player_slide) > 0) {
                player_slide = unit_vector(player_slide);
                player_slide.x *= 1000;
                player_slide.y *= 1000;
                player.apply_knockback(player_slide, 0.2);
            }

            vector_2d parry_force = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());
            if (vector_magnitude(parry_force) > 0) {
                parry_force = unit_vector(parry_force);
                parry_force.x *= 1000; 
                parry_force.y *= 1000;
                
                enemy_arr[i]->apply_knockback(parry_force, 1.0); 
            }
        }
        // 2. CHECK DAMAGE SECOND (Strict physical collision)
        else if (dist <= damage_threshold && !player.is_invincible()) {
            combo_multiplier = 1;
            combo_timer = 0;

            player.take_damage(20);
            screen_shake_timer = 0.2; 
            hit_pause_timer = 0.15;   

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
        
    for (int i = projectiles.length() - 1 ; i >= 0; i--) {
        projectiles[i].update(dt);

        // --- ADD THIS SPARK TRAIL ---
        if (projectiles[i].get_is_deflected()) {
            Particle trail_spark;
            trail_spark.base_color = COLOR_GOLD;
            trail_spark.position = projectiles[i].get_coordinates();
            // Shoot the sparks in random directions slightly
            trail_spark.velocity.x = (rnd() * 200) - 100;
            trail_spark.velocity.y = (rnd() * 200) - 100;
            trail_spark.max_lifetime = 0.2; 
            trail_spark.lifetime = trail_spark.max_lifetime;
            particles.add(trail_spark);
        }
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
            int current_size = particles[i].start_size * size_ratio;

            int alpha = 255 * size_ratio;
            if (alpha  < 0) alpha = 0;
            
            color final_color= rgba_color(
                red_of(particles[i].base_color), 
                green_of(particles[i].base_color), 
                blue_of(particles[i].base_color), 
                alpha
            ); 
            fill_rectangle(final_color, particles[i].position.x, particles[i].position.y, current_size, current_size);
        }
    }

    player.draw();

    if (show_debug) {
        player.draw_hitbox();
        draw_circle(COLOR_RED, player.get_coordinates().x, player.get_coordinates().y, 25); 
    
    // This is the 50px radius where projectiles can hurt you
    draw_circle(COLOR_BLUE, player.get_coordinates().x , player.get_coordinates().y, 20);
    }


    // ==========================================
    // --- ROOM CLEAR AND BUBBLE LOGIC ---
    // ==========================================
    if (enemy_arr.length() == 0) {

        if (level_manager.get_current_room()->get_reward() == BOSS) {
            state = VICTORY;
            stop_music();
            current_track = TRACK_NONE;
            return;
        }

        if (!current_reward.is_active && !room_cleared) {
            pause_music();
            projectiles.clear(); 
            current_reward.is_active = true;
            current_reward.position.x = screen_width() / 2;
            current_reward.position.y = screen_height() / 2;
            current_reward.type = level_manager.get_current_room()->get_reward();
        }
    }

    if (current_reward.is_active) {
        color drop_color = COLOR_WHITE;
        switch (current_reward.type) {
            case HEAL:
                drop_color = COLOR_GREEN;
                break;
            
            case ARES:
                drop_color = COLOR_RED;
                break;

            case ZEUS:
                drop_color = COLOR_YELLOW;
                break;

            case BOSS:
                break;
        }
        fill_circle(drop_color, current_reward.position.x, current_reward.position.y, 25);
        draw_circle(COLOR_WHITE, current_reward.position.x, current_reward.position.y, 27);

        if (point_point_distance(player.get_coordinates(), current_reward.position) <= 40) {
            current_reward.is_active = false;
            room_cleared = true;

            if (current_reward.type == HEAL) {
                player.heal(100);
                level_manager.generate_doors(screen_width(), screen_height());
            }
            else if (current_reward.type == ARES || current_reward.type == ZEUS) {
                populate_draft_menu(current_reward.type);
                state = DRAFTING;
            }
        }
    }

    dynamic_array<PhysicalDoor>& active_doors = level_manager.get_active_doors();
    
    for (int i = 0; i < active_doors.length(); i++) {
        fill_rectangle(COLOR_BLACK, active_doors[i].bounds);
        
        string reward_text = reward_to_string(active_doors[i].destination->get_reward());
        color symbol_color = COLOR_WHITE;
        
        // Add color coding to your doors!
        if (active_doors[i].destination->get_reward() == HEAL) symbol_color = COLOR_GREEN;
        if (active_doors[i].destination->get_reward() == ARES) symbol_color = COLOR_RED;
        if (active_doors[i].destination->get_reward() == ZEUS) symbol_color = COLOR_YELLOW;

        draw_text(reward_text, symbol_color, "TitleFont", 20, active_doors[i].bounds.x - 20, active_doors[i].bounds.y - 30);

        rectangle player_hitbox = rectangle_from(player.get_coordinates().x - 20, player.get_coordinates().y - 20, 40, 40);
        
        if (rectangles_intersect(player_hitbox, active_doors[i].bounds)) {
            level_manager.set_current_room(active_doors[i].destination);
            level_manager.clear_doors();
            level_manager.spawn_wave(enemy_arr, screen_width(), screen_height());
            
            player.set_coordinates(100, screen_height() / 2);
            
            // --- CRITICAL: Reset flags for the next room! ---
            room_cleared = false; 
            current_reward.is_active = false;
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
            play_sound_effect("Explosion", 1, 0.8f);
            screen_shake_timer = 0.3; // Violent screen shake
            hit_pause_timer = 0.1;
            
            // Did the player get caught inside the 150px radius?
            if (point_point_distance(player.get_coordinates(), danger_zones[i].position) <= danger_zones[i].radius) {
                if (!player.is_invincible()) {
                    combo_timer = 0;
                    combo_multiplier = 1;
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
        
    player.draw_hud();

    // ==========================================
    // --- ARCADE SCORE HUD (TOP RIGHT) ---
    // ==========================================
    
    double score_x = camera_x() + screen_width() - 250; 
    double score_y = camera_y() + 30;

    // 1. Draw the Base Score
    draw_text("SCORE: " + to_string(current_score), COLOR_WHITE, "TitleFont", 30, score_x, score_y);
    int minutes = (int) run_timer / 60;
    int seconds = (int) run_timer % 60;
    string time_str = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
    draw_text("TIME: " + time_str, COLOR_LIGHT_GRAY, "MenuFont", 24, score_x, score_y + 35);

    // 2. Draw the Flashy Combo Meter (Only if combo is active!)
    if (combo_multiplier > 1) {
        
        string combo_text = "x" + to_string(combo_multiplier) + " COMBO!";
        
        // Colors get more intense as the combo grows
        color combo_color = COLOR_YELLOW;
        if (combo_multiplier >= 5) combo_color = COLOR_ORANGE;
        if (combo_multiplier >= 10) combo_color = COLOR_RED; 

        // Add a slight jitter effect if you hit the max 10x combo!
        double jitter_x = (combo_multiplier >= 10) ? (rnd(4) - 2) : 0;
        double jitter_y = (combo_multiplier >= 10) ? (rnd(4) - 2) : 0;

        draw_text(combo_text, combo_color, "TitleFont", 24, score_x + 50 + jitter_x, score_y + 70 + jitter_y);

        // 3. Draw the Combo Timer Bar
       
        double timer_percent = combo_timer / 2.5; 
        if (timer_percent < 0) timer_percent = 0;

        // A dark grey background bar
        fill_rectangle(rgba_color(50, 50, 50, 200), score_x + 50, score_y + 65, 120, 8);
        
        // The shrinking colored timer bar over top
        fill_rectangle(combo_color, score_x + 50, score_y + 65, 120 * timer_percent, 8);
    }
        
    if (player.is_dead()) {
        state = DEAD;
        stop_music();
        current_track = TRACK_NONE;
    }
}

void GameController::draw_death_screen(double dt)
{
    if (current_track != TRACK_DEFEAT) {
        play_music(music_named("DefeatMusic"), 1, 0.8f);
        current_track = TRACK_DEFEAT;
    }
    death_timer += dt;
    fill_rectangle(rgba_color(20, 0, 0, 210), 0, 0, screen_width(), screen_height());

    if (death_timer > 1.5) {
        double fade_progress = (death_timer - 1.5) / 2.0;
        if (fade_progress > 1.0) fade_progress = 1.0;

        int alpha = 255 * fade_progress; // Convert progress to alpha value
        draw_text("Y O U   D I E D", rgba_color(180, 0, 0, alpha), "TitleFont", 110, screen_width() / 2 - 320, screen_height() / 2 - 100);
        
        draw_text("The Abyss consumes another soul.", rgba_color(150, 150, 150, alpha), "TextFont", 30, screen_width() / 2 - 210, screen_height() / 2 + 10);
    }

    if (death_timer > 3.5) {
        int pulse_alpha = 150 + (sine(death_timer * 150) * 100);
        draw_text("Press 'R' to Surrender and Return", rgba_color(200, 200, 200, pulse_alpha), "MenuFont", 24, screen_width() / 2 - 195, screen_height() / 2 + 120);

         if (key_typed(R_KEY)) {
            save_new_score(current_score);
            current_score = 0;
            combo_multiplier = 1;
            reset();
        }
    }
    

   
}

void GameController::draw_victory_screen(double dt)
{
    // Tick the timer forward!
    victory_timer += dt;

    // --- PHASE 1: AUDIO SWELL ---
    if (current_track != TRACK_VICTORY) {
        play_music(music_named("VictoryMusic"), 1, 0.8f);
        current_track = TRACK_VICTORY;
    }

    if (screen_shake_timer > 0) {
        screen_shake_timer -= dt;
        move_camera_to(rnd(10) - 5, rnd(10) - 5);
    } else {
        move_camera_to(0, 0);
    }

    // --- THE MATH ---
    if (!time_bonus_applied) {
        int time_bonus = 10000 - ((int)run_timer * 20);
        if (time_bonus < 0) time_bonus = 0;

        current_score += time_bonus;
        time_bonus_applied = true;
    }

    int minutes = (int)run_timer / 60;
    int seconds = (int)run_timer % 60;
    string final_time = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
    
    int display_bonus = 10000 - ((int)run_timer * 20);
    if (display_bonus < 0) display_bonus = 0;
    int stats_y = screen_height() / 2 + 30;

    // --- PHASE 2: THE ATMOSPHERE (Instant) ---
    fill_rectangle(rgba_color(0, 0, 0, 180), 0, 0, screen_width(), screen_height());

    // --- PHASE 3: THE GOLDEN TITLE (Fades in over 1.5 seconds) ---
    if (victory_timer > 0.5) {
        double progress = (victory_timer - 0.5) / 1.5;
        if (progress > 1.0) progress = 1.0;
        int alpha = 255 * progress;

        draw_text("V I C T O R Y", rgba_color(255, 215, 0, alpha), "MenuFont", 110, camera_x() + screen_width() / 2 - 340, camera_y() + screen_height() / 2 - 150);
    }

    // --- PHASE 4: THE SUBTITLE & STATS (Pops in at 2.5s) ---
    if (victory_timer > 2.5) {
        draw_text("The Titan has fallen", COLOR_WHITE, "TextFont", 30, screen_width() / 2 - 150, screen_height() / 2 - 20);
        draw_text("CLEAR TIME: " + final_time, COLOR_LIGHT_GRAY, "MenuFont", 24, screen_width() / 2 - 120, stats_y);
        draw_text("TIME BONUS: +" + to_string(display_bonus), COLOR_CYAN, "MenuFont", 24, screen_width() / 2 - 120, stats_y + 30);
    }

    // --- PHASE 5: THE FINAL SCORE SLAM (Explodes in at 4.0s) ---
    if (victory_timer > 4.0) {
        
        // Trigger the bass hit exactly ONCE!
        // --- NEW: THE SCORE COUNT-UP MATH ---
        // Ticks up over exactly 1.5 seconds
        double count_progress = (victory_timer - 4.0) / 1.5; 
        if (count_progress > 1.0) count_progress = 1.0;
        
        int display_score = current_score * count_progress;
        
        // Flicker the text red while counting, snap to solid yellow when done!
        color score_col = COLOR_YELLOW;
        if (count_progress < 1.0) {
            // Flickers every few frames
            if ((int)(victory_timer * 20) % 2 == 0) score_col = COLOR_RED; 
            else score_col = COLOR_WHITE;
            
            // Add a ticking sound using your light sword swing!
            if ((int)(victory_timer * 20) % 3 == 0) play_sound_effect("Sword", 1, 0.1f); 
        }

        draw_text("FINAL SCORE: " + to_string(display_score), score_col, "TitleFont", 35, camera_x() + screen_width() / 2 - 140, stats_y + 80);
    }

    // --- PHASE 6: THE INPUT (Pulses at 6.0s) ---
    if (victory_timer > 6.0) {
        int pulse = 150 + (sine(victory_timer * 150) * 100);
        draw_text("Press 'R' to Return to Hub", rgba_color(200, 200, 200, pulse), "MenuFont", 30, screen_width() / 2 - 190, screen_height() / 2 + 180);

        if (key_typed(R_KEY)) {
            save_new_score(current_score);
            reset();
        }
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
    victory_timer = 0;
    score_sound_played = false;

    player.reset_stats();
    death_timer = 0;
    current_score = 0;
    combo_multiplier = 1;
    run_timer = 0;
    time_bonus_applied = false;
    hit_pause_timer = 0;
    screen_shake_timer = 0;
    arena_radius = 1200.0;

    // Send the player back to the safe zone
    state = HUB;
    player.set_coordinates(screen_width() / 2, screen_height() - 100); 
}

string GameController::reward_to_string(reward_type r) {
    if (r == HEAL)   return "HEAL +100 HP";
    if (r == ZEUS)  return "SPEED BOOST";
    if (r == ARES) return "DAMAGE BOOST";
    if (r == BOSS)   return "BOSS ROOM";
    return "???";
}

void GameController::apply_reward(reward_type reward)
{
    if (reward == HEAL) {
        player.heal(100);
    }
    else if (reward == ZEUS) {
        player.increase_speed(50);
    }
    else if (reward == ARES) {
        player.increase_damage(50);
    }
}

void GameController::populate_draft_menu(reward_type god_type)
{
    current_draft_options.clear();

    if (god_type == ARES) { 
        // Ares Theme: High raw damage, violent speed
        current_draft_options.add({BOON_ARES_STRIKE, "Blood Strike", "+40 Sword Damage.", COLOR_DARK_RED});
        current_draft_options.add({BOON_ARES_SWIFTNESS, "War Frenzy", "+150 Movement Speed.", COLOR_RED});
    } 
    else if (god_type == ZEUS) { 
        // Zeus Theme: Lower raw damage, but extreme speed and chain lightning
        current_draft_options.add({BOON_ZEUS_STRIKE, "Lightning Strike", "+15 Damage & Sparks.", COLOR_YELLOW});
        current_draft_options.add({BOON_ZEUS_SWIFTNESS, "Tailwind", "+250 Movement Speed.", COLOR_LIGHT_BLUE});
        current_draft_options.add({BOON_ZEUS_DASH, "Thunder Dash", "Dash strikes nearby.", COLOR_WHITE});
    }
}

void GameController::update_drafting()
{
    // 1. Draw a dark semi-transparent overlay over the paused game
    fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, screen_width(), screen_height());
    string title_txt = "CHOOSE YOUR BLESSING";
    font t_font = font_named("TitleFont");
    double title_w = text_width(title_txt, t_font, 60);
    draw_text(title_txt, COLOR_WHITE, t_font, 60, (screen_width() / 2.0) - (title_w / 2.0), 100);

    // 2. Draw 3 Boon Cards side-by-side
    int card_width = 250;
    int card_height = 350;
    int start_x = (screen_width() - (card_width * 3 + 100)) / 2; // Center them perfectly

    for (int i = 0; i < current_draft_options.length(); i++) {
        int x = start_x + (i * (card_width + 50));
        int y = 250;
        rectangle card_rect = rectangle_from(x, y, card_width, card_height);

        // Draw the card background and colored border
        fill_rectangle(rgba_color(30, 30, 30, 255), card_rect);
        draw_rectangle(current_draft_options[i].theme, card_rect);

        // Draw the Boon text
        draw_text(current_draft_options[i].name, current_draft_options[i].theme, "TitleFont", 24, x + 20, y + 20);
        draw_text(current_draft_options[i].description, COLOR_WHITE, "TextFont", 16, x + 20, y + 80);

        // 3. Selection Logic (Did the player click this card?)
        if (mouse_clicked(LEFT_BUTTON)) {
            if (point_in_rectangle(mouse_position(), card_rect)) {
                
                // Give the player the boon!
                player.add_boon(current_draft_options[i]);

                // Spawn the exit doors NOW that a choice was made!
                level_manager.generate_doors(screen_width(), screen_height());
                
                state = PLAYING; // Unpause the game!
            }
        }
    }
}

void GameController::update_main_menu()
{
    // 1. THE ATMOSPHERE (Deep Blood Red / Obsidian Black)
    fill_rectangle(rgba_color(15, 5, 5, 255), 0, 0, screen_width(), screen_height());



    // 3. THE TITLE
    // Using COLOR_CRIMSON and spreading the letters out makes it look majestic
    draw_text("P R O J E C T   T I T A N", COLOR_CRIMSON, "MenuFont", 80, 100, 150);

    // ==========================================
    // --- ELEGANT HOVER BUTTONS ---
    // ==========================================
    
    // We use invisible rectangles just for the math, but we don't draw them!
    rectangle start_rect = rectangle_from(100, 350, 300, 50);
    rectangle quit_rect = rectangle_from(100, 450, 300, 50);

    // Check where the mouse is
    bool hover_start = point_in_rectangle(mouse_position(), start_rect);
    bool hover_quit = point_in_rectangle(mouse_position(), quit_rect);

    // If hovered, the text turns GOLD. If not, it stays a dark grey/white.
    color start_color = hover_start ? COLOR_GOLD : COLOR_LIGHT_GRAY;
    color quit_color = hover_quit ? COLOR_GOLD : COLOR_LIGHT_GRAY;

    // Draw the text (Notice the spaces between letters for that cinematic look!)
    draw_text("B E G I N   E S C A P E", start_color, "TitleFont", 35, start_rect.x, start_rect.y);
    draw_text("S U R R E N D E R", quit_color, "TitleFont", 35, quit_rect.x, quit_rect.y);

    // ==========================================
    // --- CLICK LOGIC ---
    // ==========================================
    if (mouse_clicked(LEFT_BUTTON)) {
        if (hover_start) {
            player.reset_stats(); 
            state = HUB; 
        }
        else if (hover_quit) {
            // A quick hack to force the game to close from inside the controller!
            exit(0); 
        }
    }
}

void GameController::draw_pause_menu()
{
    // 1. Darken the screen behind the menu
    fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, screen_width(), screen_height());

    // 2. Draw the Title
    draw_text("P A U S E D", COLOR_WHITE, "TitleFont", 70, screen_width() / 2 - 200, 200);

    // 3. Setup the Invisible Button Zones
    rectangle resume_btn = rectangle_from(screen_width() / 2 - 150, 350, 300, 50);
    rectangle main_btn = rectangle_from(screen_width() / 2 - 150, 430, 300, 50);
    rectangle quit_btn = rectangle_from(screen_width() / 2 - 150, 510, 300, 50);

    bool hover_res = point_in_rectangle(mouse_position(), resume_btn);
    bool hover_main = point_in_rectangle(mouse_position(), main_btn);
    bool hover_quit = point_in_rectangle(mouse_position(), quit_btn);

    // 5. Draw the Glowing Text
    draw_text("R E S U M E", hover_res ? COLOR_GOLD : COLOR_LIGHT_GRAY, "MenuFont", 35, resume_btn.x + 35, resume_btn.y);
    draw_text("M A I N   M E N U", hover_main ? COLOR_GOLD : COLOR_LIGHT_GRAY, "MenuFont", 35, main_btn.x, main_btn.y);
    draw_text("Q U I T   G A M E", hover_quit ? COLOR_GOLD : COLOR_LIGHT_GRAY, "MenuFont", 35, quit_btn.x + 10, quit_btn.y);

    // 6. Click Logic
    if (mouse_clicked(LEFT_BUTTON)) {
        if (hover_res) {
            is_paused = false;
            resume_music();
    
        } 
        else if (hover_main) {
            // Return to Main Menu safely!
            is_paused = false;
            state = MAIN_MENU;
            stop_music();
            current_track = TRACK_NONE;
        } 
        else if (hover_quit) {
            exit(0);
        }
    }
}

void GameController::load_leaderboard()
{
    std::ifstream file("leaderboard.txt");

    if (file.is_open()) {
        for (int i = 0; i < 5; i++) {
            file >> top_scores[i];
        }
        file.close();
    } else {

    }
}

void GameController::save_new_score(int final_score)
{
    int insert_index = -1;

    // Check if new score deserves to be on the board
    for (int i = 0; i < 5; ++i) {
        if (final_score > top_scores[i]) {
            insert_index = i;
            break;
        }
    }

    if (insert_index != -1) {
        for (int i = 4; i > insert_index; --i) {
            top_scores[i] = top_scores[i - 1];
        }
        top_scores[insert_index] = final_score;

        std::ofstream file("leaderboard.txt");
        if (file.is_open()) {
            for (int i = 0; i < 5; i++) {
                file << top_scores[i] << std::endl;
            }
            file.close();
        }
    }
}

#include "enemy.h"
#include <cmath>

Enemy::Enemy(double x, double y, int assigned_id, double health, double speed)
    : Entity(x, y, health, speed)
{
    id = assigned_id;
    reload_timer = 1;
}

Enemy::~Enemy() {}

void Enemy::draw(bool show_debug)
{

    double hp_percent = health / max_health;

    if (hp_percent < 0)
    {
        hp_percent = 0;
    }

    fill_rectangle(COLOR_BLACK, coordinates.x - 20, coordinates.y - 20, 40, 5);
    fill_rectangle(COLOR_GREEN, coordinates.x - 20, coordinates.y - 20, 40 * hp_percent, 5);
}

void Enemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    (void)target;
    (void)projectiles;

    // Part 1: Physics
    coordinates.x += velocity.x * dt;
    coordinates.y += velocity.y * dt;

    velocity.x -= velocity.x * 8.0 * dt;
    velocity.y -= velocity.y * 8.0 * dt;

    if (flash_timer > 0)
    {
        flash_timer -= dt;
    }

    if (stun_timer > 0)
    {
        stun_timer -= dt;
    }

    int border = 60;
    if (coordinates.x < border) coordinates.x = border;
    if (coordinates.x > screen_width() - border) coordinates.x = screen_width() - border;
    if (coordinates.y < border) coordinates.y = border;
    if (coordinates.y > screen_height() - border) coordinates.y = screen_height() - border;
}

ShooterEnemy::ShooterEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 80, 250)
{
    enemy_anim = create_animation(animation_script_named("HellhoundAnim"), "Idle");
}

void ShooterEnemy::draw(bool show_debug)
{
    drawing_options opt = option_with_animation(enemy_anim);

    if (is_facing_left) {
        opt.flip_y = true;
    }

    if (flash_timer > 0)
    {
        fill_circle(COLOR_WHITE, coordinates, 20);
    }
    else
    {
        draw_bitmap("Hellhound", coordinates.x - 40, coordinates.y - 30, opt);
    }

    if (is_winding_up)
    {
        double flash_rate = 0.1 - (0.07 * (1 - windup_timer / 0.3));
        if (std::fmod(windup_timer, flash_rate * 2) < flash_rate)
        {
            draw_text("!", COLOR_ORANGE, "TitleFont", 24, coordinates.x - 5, coordinates.y - 45);
        }
    }

    Enemy::draw(show_debug);
}

void ShooterEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles);
    
    if (target.get_coordinates().x < coordinates.x) {
        is_facing_left = false;
    } else {
        is_facing_left = true;
    }
    if (stun_timer > 0)
    {
        return;
    }

    // AI: Walk to player
    double dist = point_point_distance(coordinates, target.get_coordinates());
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());

  
    if (vector_magnitude(raw_dir) > 0)
    {
        vector_2d move_dir = unit_vector(raw_dir);

        if (!is_winding_up) {
            if (dist > 150) {
                coordinates.x += move_dir.x * movement_speed * dt;
                coordinates.y += move_dir.y * movement_speed * dt;
            } else {
                int strafe_dir = (get_id() % 2 == 0) ? 1 : -1; 
                
                // The magical Perpendicular Vector math (-Y, X)
                double side_x = -move_dir.y * strafe_dir;
                double side_y = move_dir.x * strafe_dir;

                // Move sideways at 70% speed so the player can still easily catch them
                double strafe_speed = movement_speed * 0.6; 
                
                coordinates.x += side_x * strafe_speed * dt;
                coordinates.y += side_y * strafe_speed * dt;
            }
        }
        
    }

    // --- ANIMATION SWITCH (same pattern as guide's assign_animation calls) ---
    string desired_anim = (stun_timer > 0 || is_winding_up) ? "Idle" : "Run";

    if (desired_anim != current_anim_name)
    {
        current_anim_name = desired_anim;
        assign_animation(enemy_anim, desired_anim, animation_script_named("HellhoundAnim"));
    }

    // Always update the animation every frame
    update_animation(enemy_anim);

    // AI: Shooting Logic
    if (reload_timer <= 0)
    {
        if (!is_winding_up) {
            is_winding_up = true;
            windup_timer = 0.3;
        }
        else{
            windup_timer -= dt;
            if (windup_timer <= 0)
            {
                projectiles.add(Projectile(coordinates, target.get_coordinates()));
                reload_timer = 1;
                is_winding_up = false;
            }
        }
    } else {
        reload_timer -= dt;
    }
}

ChaserEnemy::ChaserEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 50, 300) {
        chaser_anim = create_animation(animation_script_named("ChaserAnim"), "Run");
        current_anim_name = "Run";
        is_facing_left = false;
    }

void ChaserEnemy::draw(bool show_debug)
{
    drawing_options opt = option_with_animation(chaser_anim);
    if (is_facing_left) {
        opt.flip_y = true;
    }

    if (flash_timer > 0)
        fill_circle(COLOR_WHITE, coordinates, 20);
    else {
        double draw_x = coordinates.x - 48;
        double draw_y = coordinates.y - 56;

        draw_bitmap("ChaserSprite", draw_x, draw_y , opt);
    }

    Enemy::draw(show_debug); // Draw the health bar!
}

void ChaserEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles); // Run Base Physics

    // Face player
    if (target.get_coordinates().x < coordinates.x) {
        is_facing_left = true;
    } else {
        is_facing_left = false;
    }
    if (stun_timer > 0)
        return; // Abort AI if stunned!

    // AI: Sprint at player
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0)
    {
        vector_2d move_dir = unit_vector(raw_dir);

        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }

    if (dt > 0) {
        update_animation(chaser_anim);
    }
}

BossEnemy::BossEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 4000, 150)
{
    current_state = B_INTRO;
    state_timer = 2.5;
    action_timer = 0;
    burst_counter = 0;
    boss_anim = create_animation(animation_script_named("BossAnim"), "Idle");
    current_anim_name = "Idle";
    
}

void BossEnemy::draw(bool show_debug)
{ 
    // Draw the Telegraphs based onhe State!
    if (current_state == B_GROUND_SLAM && action_timer > 0) {
        double max_time = is_enraged ? 1.0 : 1.5;
        double progress = 1.0 - (action_timer / max_time);
        
        fill_circle(rgba_color(255, 0, 0, 30), coordinates.x, coordinates.y, 200);
        fill_circle(rgba_color(255, 0, 0, 120), coordinates.x, coordinates.y, 200 * progress);

    } else if (current_state == B_PHASE_TRANSITION) {
        double progress = 1.0 - (state_timer / 2.5);
        if (progress > 1.0) progress = 1.0;
        
        // Dim the lights!
        fill_rectangle(rgba_color(0.0, 0.0, 0.0, 100 * progress), camera_x(), camera_y(), screen_width(), screen_height());
        
        // Draw the massive ticking time-bomb circle
        fill_circle(rgba_color(255, 0, 0, 30), coordinates.x, coordinates.y, 200);
        fill_circle(rgba_color(255, 0, 0, 150), coordinates.x, coordinates.y, 200 * progress);
        
        // Draw a blinding inner aura that throbs
        fill_circle(rgba_color(255, 50, 50, 100), coordinates.x, coordinates.y, 80 + (rnd() * 30));
    } 
    else if (current_state == B_SHADOW_STEP) {
        
        // 1. THE BLINKING LIGHT CUES (Only happens on Beats 1 and 2)
        // Check if we are in the first 0.15s of the beat
        fill_rectangle(rgba_color(0, 0, 0, 210), camera_x(), camera_y(), screen_width(), screen_height());

    double cx = camera_x() + (screen_width() / 2.0);
    double cy = camera_y() + (screen_height() / 2.0);

    // 2. THE TELEGRAPHS (Burst 1, 3, 5)
    if (burst_counter == 1) { 
        // Slash 1 Cue
        if (std::fmod(action_timer, 0.1) > 0.05) { // Flickering warning
            for (int offset = -400; offset <= 400; offset += 400) {
                for(int i = -2; i <=2 ;i++) {
                    draw_line(COLOR_YELLOW, cx - 800, cy - 500 + offset + i, cx + 800, cy + 500 + offset+ i);
                }
            }
        }
        draw_text("PARRY!", COLOR_YELLOW, "TitleFont", 40, cx + 100, cy - 100);
    }
    else if (burst_counter == 3) {
        // Slash 2 Cue: Top-Right to Bottom-Left
        if (std::fmod(action_timer, 0.1) > 0.05) {
            for (int offset = -400; offset <= 400; offset += 400) {
                for (int i = -2; i <= 2; i++) {
                    draw_line(COLOR_YELLOW, cx + 800, cy - 500 + offset + i, cx - 800, cy + 500 + offset + i);
                }
            }
        }
        draw_text("PARRY!", COLOR_YELLOW, "TitleFont", 40, cx - 200, cy + 50);
    }
    else if (burst_counter == 5) {
        // Final Cut Cue: Massive Horizontal Beam
        if (std::fmod(action_timer, 0.1) > 0.05) {
            
            fill_rectangle(rgba_color(255, 0, 0, 150), camera_x(), cy - 40, screen_width(), 80);
            draw_rectangle(COLOR_RED, camera_x(), cy - 40, screen_width(), 80);
            
            // Generate a random offset between -3 and +3 pixels
            double jitter_x = rnd(6) - 3;
            double jitter_y = rnd(6) - 3;
            
            // Text shakes violently to sell the charging power!
            draw_text("FINAL STRIKE", COLOR_RED, "TitleFont", 50, cx - 160 + jitter_x, cy - 90 + jitter_y);
        }
    }

    // 3. THE ACTIVE HITS (Burst 2, 4, 6)
    if (burst_counter == 2) {
        // FULL SCREEN FLASH (Tints the entire room blood red to show the AoE is global)
        fill_rectangle(rgba_color(150, 0, 0, 100), camera_x(), camera_y(), screen_width(), screen_height());

        // Thick Red/White Slashes x3
        for (int offset = -400; offset <= 400; offset += 400) {
            for (int i = -15; i <= 15; i++) {
                draw_line(COLOR_RED, cx - 800, cy - 500 + offset + i, cx + 800, cy + 500 + offset + i);
            }
            for (int i = -4; i <= 4; i++) {
                draw_line(COLOR_WHITE, cx - 800, cy - 500 + offset + i, cx + 800, cy + 500 + offset + i);
            }
        }
    }
    else if (burst_counter == 4) {
        // FULL SCREEN FLASH
        fill_rectangle(rgba_color(150, 0, 0, 100), camera_x(), camera_y(), screen_width(), screen_height());

        // Thick Red/White Slashes x3 (Flipped)
        for (int offset = -400; offset <= 400; offset += 400) {
            for (int i = -15; i <= 15; i++) {
                draw_line(COLOR_RED, cx + 800, cy - 500 + offset + i, cx - 800, cy + 500 + offset + i);
            }
            for (int i = -4; i <= 4; i++) {
                draw_line(COLOR_WHITE, cx + 800, cy - 500 + offset + i, cx - 800, cy + 500 + offset + i);
            }
        }
    }
    else if (burst_counter == 6) {
        // Blinding Screen Nuke
       if (std::fmod(action_timer, 0.08) > 0.04) {
            // Frame A: Blinding White screen with a massive Red tear
            fill_rectangle(COLOR_WHITE, camera_x(), camera_y(), screen_width(), screen_height());
            fill_rectangle(COLOR_DARK_RED, camera_x(), cy - 100, screen_width(), 200);
        } else {
            // Frame B: Pitch Black screen with a thin White tear
            fill_rectangle(rgba_color(10, 10, 15, 255), camera_x(), camera_y(), screen_width(), screen_height());
            fill_rectangle(COLOR_WHITE, camera_x(), cy - 20, screen_width(), 40);
            }
        }
    }

    // 2. Draw the actual Boss Sprite!
    drawing_options opts = option_with_animation(boss_anim);
    if (is_facing_left) {
        opts.flip_y = true;
    }
    opts.scale_x = 2.5;
    opts.scale_y = 2.5;

    double draw_x = coordinates.x - 200;
    double draw_y = coordinates.y - 150;

    // --- TWEAK THESE OFFSETS ---
    // If the hitbox is too far to the right, it means the sprite is drawing too far to the left.
    // Add positive X to push the sprite to the right!
    double offset_x = 130; // Tweak this number! 
    double offset_y = 10; // Tweak this number!

    draw_x += offset_x;
    draw_y += offset_y;

    if (flash_timer > 0) {
        draw_x += (rnd() * 16- 8);
        draw_y += (rnd() * 16 - 8);
    }
    
    // Draw the boss perfectly centered on its collision coordinates
    if (current_state != B_SHADOW_STEP) {
        draw_bitmap("Boss", draw_x, draw_y, opts);
    }

    // ANIME GLINT
    if (current_state == B_MELEE_COMBO && action_timer > 0) {
        if (std::fmod(action_timer, 0.1) > 0.05) {
            double glint_x = is_facing_left ? coordinates.x - 40 : coordinates.x + 40;
            fill_circle(COLOR_WHITE, glint_x, coordinates.y - 20, 15);
            fill_circle(COLOR_YELLOW, glint_x, coordinates.y - 20, 10);

            fill_rectangle(COLOR_WHITE, glint_x - 30, coordinates.y - 22, 60, 4);
            fill_rectangle(COLOR_WHITE, glint_x - 2, coordinates.y - 50, 4, 60);
        }
    }

    // ==========================================
    // --- FLOATING COMBAT TEXT ---
    // ==========================================
    string attack_text = "";
    color text_color = COLOR_WHITE;

    if (current_state == B_TRIPLE_BURST) {
        attack_text = "TRIPLE BURST!";
        text_color = COLOR_RED;
    } 
    else if (current_state == B_SHOTGUN) {
        attack_text = "SHOTGUN!";
        text_color = COLOR_ORANGE;
    } 
    else if (current_state == B_LUNGE) {
        attack_text = "LUNGE!";
        text_color = COLOR_YELLOW;
    } 
    else if (current_state == B_MELEE_COMBO) {
        attack_text = "COMBO!";
        text_color = COLOR_DARK_RED;
    }

    // Only draw the text if the boss is actually attacking
    if (attack_text != "") {
        draw_text(attack_text, text_color, "TitleFont", 20, coordinates.x - 45, coordinates.y - 85);
    }

    // ==========================================
    // --- THE GRAND BOSS HEALTH BAR ---
    // ==========================================
    
    double active_hp = (current_state == B_INTRO) ? display_health : health;
    double hp_percent = active_hp / max_health;
    if (hp_percent < 0) hp_percent = 0;

    double screen_center = camera_x() + (screen_width() / 2.0);
    double bar_y = camera_y() + 60; // Pushed down slightly to give the text room
    
    double bar_width = 800;
    double start_x = screen_center - (bar_width / 2.0);

    if (current_state == B_INTRO) {
        // As the timer counts down, the screen gets darker
        double intensity = state_timer / 2.5; 
        
        // 1. Dim the whole screen
        fill_rectangle(rgba_color(0.0, 0.0, 0.0, 180 * intensity), camera_x(), camera_y(), screen_width(), screen_height());
        
        // 2. Pulse a violent, jittering crimson aura around the crouched boss
        double aura_radius = 80 + (rnd() * 20); 
        fill_circle(rgba_color(150.0, 0.0, 0.0, 100 * intensity), coordinates.x, coordinates.y, aura_radius);
    }

    // --- DRAW THE BAR ---
    fill_rectangle(rgba_color(15, 5, 5, 255), start_x, bar_y, bar_width, 20);
    draw_rectangle(COLOR_DARK_GRAY, start_x - 2, bar_y - 2, bar_width + 4, 24);
    fill_rectangle(COLOR_RED, start_x, bar_y, bar_width * hp_percent, 20);

    // --- PERFECTLY CENTERED TITLE ---
    string boss_title = "V A E L E N ,   T H E   A B Y S S   W A T C H E R";
    int font_size = 26;
    font title_font = font_named("TitleFont");
    
    // Let SplashKit calculate the exact pixel width of the string
    double text_w = text_width(boss_title, title_font, font_size);
    
    // Shift the text left by exactly half its width
    draw_text(boss_title, COLOR_WHITE, title_font, font_size, screen_center - (text_w / 2.0), bar_y - 35);

    if (show_debug) {
        // A bright green ring showing the actual 120px collision zone
        draw_circle(COLOR_LIME_GREEN, coordinates.x, coordinates.y, get_collision_radius());
        
        // A red dot showing the exact mathematical center of the boss
        fill_circle(COLOR_RED, coordinates.x, coordinates.y, 5);
    }
}



void BossEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles);
    if (target.get_coordinates().x < coordinates.x) {
        is_facing_left = true;
    } else is_facing_left = false;


    if (ultimate_cooldown > 0) {
        ultimate_cooldown -= dt;
    }
    if (!is_enraged && health <= max_health / 2) {
        is_enraged = true;
        current_state = B_PHASE_TRANSITION;
        play_sound_effect("BossRoar", 1, 0.7f);
        state_timer = 3.0;
        action_timer = 0;
        movement_speed = 250;
        ultimate_cooldown = 0;
    }

    double downtime = is_enraged ? 1.0 : 2.0;

    // ==========================================
    // --- THE FINITE STATE MACHINE (FSM) ---
    // ==========================================
    switch (current_state)
    {
    
    case B_INTRO:
    {
        if (display_health < max_health) {
            // Fill rate = Total Health / Total Time
            display_health += (max_health / 2.5) * dt; 
        }

        // 3. Countdown to the fight
        state_timer -= dt;
        if (state_timer <= 0) {
            display_health = max_health; // Snap to full just in case
            current_state = B_WALKING;   // Let the battle begin!
            state_timer = downtime;
        }
        break;
    }
    case B_WALKING:
    {

        if (is_enraged && ultimate_cooldown <= 0 && point_point_distance(coordinates, target.get_coordinates()) < 300) {
            current_state = B_SHADOW_STEP;
            action_timer = 0.5;
            burst_counter = 0;
            ultimate_cooldown = 12.0;

            if (current_anim_name != "Idle") {
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            }
            
            break;
        }

        if (stun_timer > 0) {
            if (current_anim_name != "Hurt") {
                assign_animation(boss_anim, "Hurt");
                current_anim_name = "Hurt";
            }
        }
        else if (current_anim_name != "Run") {
            assign_animation(boss_anim, "Run");
            current_anim_name = "Run";
        }
        // Simply walk toward the player
        vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
        if (vector_magnitude(raw_dir) > 0)
        {
            vector_2d move_dir = unit_vector(raw_dir);
            coordinates.x += move_dir.x * movement_speed * dt;
            coordinates.y += move_dir.y * movement_speed * dt;
        }

        // Count down to the next attack
        state_timer -= dt;
        if (state_timer <= 0)
        {
            // Roll a 3-sided dice to pick a random attack
            float roll = rnd();
            if (roll < 0.20)
            {
                current_state = B_TRIPLE_BURST;
                action_timer = is_enraged ? 0.3 : 0.5;
                burst_counter = 0;
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            }
            else if (roll < 0.40)
            {
                current_state = B_SHOTGUN;
                action_timer = is_enraged ? 0.4: 0.6; // 0.6s Telegraph
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            }
            else if (roll < 0.60)
            {
                current_state = B_LUNGE;
                action_timer = is_enraged ? 0.5 : 0.8; // 0.8s Telegraph
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";

            } else if (roll < 0.80){
                current_state = B_MELEE_COMBO;
                action_timer = is_enraged ? 0.3 : 0.5;
                burst_counter = 0;
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            } else {
                current_state = B_GROUND_SLAM;
                action_timer = is_enraged ? 0.5 : 1.5;
                assign_animation(boss_anim, "JumpWindup");
                current_anim_name = "JumpWindup";
            }
        }
        break;
    }

    case B_TRIPLE_BURST:
    {
        action_timer -= dt;
        if (action_timer <= 0)
        {

            // Fire a ring of 12 projectiles
            for (int i = 0; i < 12; i++)
            {
                // Offset the angle slightly for each burst wave (burst_counter * 15 degrees)
                double angle = (i * 30) + (burst_counter * 15);

                point_2d proj_target;
                proj_target.x = coordinates.x + cosine(angle) * 100;
                proj_target.y = coordinates.y + sine(angle) * 100;
                projectiles.add(Projectile(coordinates, proj_target));
            }

            burst_counter++;
            int max_waves = is_enraged ? 5 : 3;
            if (burst_counter >= max_waves)
            {
                // Finished firing 3 waves, go back to walking!
                current_state = B_WALKING;
                state_timer = downtime;
            }
            else
            {
                // Wait 0.3 seconds before firing the next ring
                action_timer = is_enraged ? 0.15 : 0.3;
            }
        }
        break;
    }

    case B_SHOTGUN:
    {
        action_timer -= dt;
        if (action_timer <= 0)
        {
            // 1. Find the exact angle to the player
            vector_2d dir = vector_point_to_point(coordinates, target.get_coordinates());
            double base_angle = vector_angle(dir);
            int spread = is_enraged ? 6 : 3;

            // 2. Fire 7 bullets in a cone spread
            for (int i = -spread; i <= spread; i++)
            {
                double angle = base_angle + (i * 10); // Spread bullets 10 degrees apart

                point_2d proj_target;
                proj_target.x = coordinates.x + cosine(angle) * 100;
                proj_target.y = coordinates.y + sine(angle) * 100;
                projectiles.add(Projectile(coordinates, proj_target));
            }

            current_state = B_WALKING;
            state_timer = downtime;
        }
        break;
    }

    case B_LUNGE:
    {
        if (action_timer > 0)
        {
            // Phase 1: Telegraphing (Standing perfectly still)
            action_timer -= dt;

            if (action_timer >= 0.2)
            {
                point_2d p_pos = target.get_coordinates();
                vector_2d p_vel = target.get_curr_velocity();
                locked_target.x = p_pos.x + (p_vel.x * 0.15);
                locked_target.y = p_pos.y + (p_vel.y * 0.15);
            }

            if (action_timer <= 0)
            {
                assign_animation(boss_anim, "NormalAttack");
                current_anim_name = "NormalAttack";
                vector_2d raw_dir = vector_point_to_point(coordinates, locked_target);
                if (vector_magnitude(raw_dir) > 0)
                {
                    raw_dir = unit_vector(raw_dir);
                    int lunge_spd = is_enraged ? 2500 : 2000;

                    velocity.x = raw_dir.x * lunge_spd;
                    velocity.y = raw_dir.y * lunge_spd;
                    play_sound_effect("BossLunge");
                }
                state_timer = 0.7;
            }
        }
        else
        {
            state_timer -= dt;
            if (state_timer <= 0)
            {
                current_state = B_WALKING;
                state_timer = downtime;
            }
        }
        break;
    }

    case B_MELEE_COMBO:
    {
        // PHASE 1: The Initial Telegraph 
        // (Boss stands still in Idle before starting the flurry)
        if (action_timer > 0) {
            action_timer -= dt;
            if (action_timer <= 0) {
                // Telegraph is over! Force the first swing to trigger instantly.
                state_timer = 0; 
            }
        } 
        // PHASE 2: The Relentless Flurry
        else {
            state_timer -= dt; 
            
            // The exact millisecond a swing finishes (or on the very first swing):
            if (state_timer <= 0) {
                int max_combo = is_enraged ? 6 : 3;

                if (burst_counter >= max_combo) {
                    // Combo complete! Walk away.
                    current_state = B_WALKING;
                    state_timer = downtime;
                } 
                else {
                    // === INSTANT NEXT SWING (ZERO COOLDOWN) ===
                    if (burst_counter % 2 == 0) {
                        assign_animation(boss_anim, "NormalAttack");
                        current_anim_name = "NormalAttack";
                        play_sound_effect("BossSwing", 1, 0.7f);
                    } else {
                        assign_animation(boss_anim, "CrouchAttack");
                        current_anim_name = "CrouchAttack";
                        play_sound_effect("BossSwing", 1, 0.7f);
                    }
                    
                    // Aim at the player
                    point_2d p_pos = target.get_coordinates();
                    vector_2d p_vel = target.get_curr_velocity();
                    locked_target.x = p_pos.x + (p_vel.x * 0.15);
                    locked_target.y = p_pos.y + (p_vel.y * 0.15);
                    
                    // Dash forward
                    vector_2d raw_dir = vector_point_to_point(coordinates, locked_target);
                    if (vector_magnitude(raw_dir) > 0) {
                        raw_dir = unit_vector(raw_dir);
                        int dash_spd = is_enraged ? 2500: 2000;
                        velocity.x = raw_dir.x * dash_spd;
                        velocity.y = raw_dir.y * dash_spd;
                    }

                    // --- SWORD SWING SPARKS ---
                    for (int p = 0; p < 20; ++p) {
                        Particle spark;
                        spark.position = coordinates;
                        spark.base_color = COLOR_YELLOW;
                        double spread = (rnd() * 90) - 45; // Wider arc
                        double angle = vector_angle(raw_dir) + spread;
                        
                        // Slow drift
                        double speed = 50 + (rnd() * 150); 
                        spark.velocity.x = cosine(angle) * speed;
                        spark.velocity.y = sine(angle) * speed;
                        
                        // Lingers behind him
                        spark.max_lifetime = 0.6 + (rnd() * 0.4); 
                        spark.lifetime = spark.max_lifetime;
                        
                        // Pitch black shadows
                        spark.base_color = rgba_color(10, 10, 15, 255); 
                        particles.add(spark);
                    }
                    
                    burst_counter++; 
                    
                    
                    state_timer = 0.7; 
                }
            }
        }
        break;
    }
    case B_GROUND_SLAM:
    {
        if (action_timer > 0) {
            action_timer -= dt;

            if (action_timer <= 0) {
                trigger_aoe = true;
                play_sound_effect("BossSlam", 1, 0.6f);
                assign_animation(boss_anim, "JumpSlam");
                current_anim_name = "JumpSlam";
                state_timer = 0.6;

                // --- THE ABYSSAL SLAM ERUPTION ---
                for (int p = 0; p < 100; ++p) { // Increased count for a denser cloud
                    Particle dust;
                
                    dust.start_size = 30 + (rnd() * 20);
                    
                    double angle = (360.0/100) * p;
                    double blast_radius = 200.0;
                    dust.position.x = coordinates.x + cosine(angle) *blast_radius;
                    dust.position.y = coordinates.y + sine(angle) * blast_radius;

                    dust.position.y += 40;
                    // Slower speed 
                    double speed = 50 + (rnd() * 100); 
                    dust.velocity.x = cosine(angle) * speed;
                    dust.velocity.y = sine(angle) * speed;
                    
                    // Lingers for a massive 1.5 seconds
                    dust.max_lifetime = 1.0 + (rnd() * 0.5); 
                    dust.lifetime = dust.max_lifetime;
                    
                    // Deep, dark purple/black color
                    dust.base_color = rgba_color(25, 5, 35, 255); 
                    particles.add(dust);
                }
            }
        } else {
            state_timer -=dt;
            if (state_timer <= 0) {
                current_state = B_WALKING;
                state_timer = downtime;
            }
        }
        break;
    }

    case B_PHASE_TRANSITION:
    {
        if (health < max_health / 2) {
            health = max_health / 2;
        }
        
        if (current_anim_name != "Idle") {
            assign_animation(boss_anim, "Idle");
            current_anim_name = "Idle";
        }

        Particle aura;
        aura.position.x = coordinates.x + (rnd() * 80 - 40);
        aura.position.y = coordinates.y + (rnd() * 80 - 40);
        aura.velocity.y = -150 - (rnd() * 150); // Float upwards fast
        aura.velocity.x = (rnd() * 100) - 50;
        aura.max_lifetime = 0.4 + rnd() * 0.4;
        aura.lifetime = aura.max_lifetime;
        aura.base_color = rgba_color(200, 0, 0, 255); // Bright blood red
        aura.start_size = 15;
        particles.add(aura);

        state_timer -= dt;
        if (state_timer<= 0) {
            current_state = B_WALKING;
            state_timer = downtime;
        }
        break;
    }

    case B_SHADOW_STEP:
    {
        action_timer -= dt;
        if (action_timer <= 0) {
            burst_counter++;

            if (burst_counter == 1 || burst_counter == 3 || burst_counter == 5) {
                play_sound_effect("Clink", 1, 0.7f);
                
                // 0.3s after the first warning, but a terrifying 0.6s gap after the second!
                action_timer = 0.5;
            }
            // BEATS 3 & 4: THE STRIKES (Slash! Slash!)
            else if (burst_counter == 2 || burst_counter == 4) {
                play_sound_effect("BossSlam", 1, 0.8f);
                action_timer = 0.15; // Lightning fast 0.3s follow-up slash!
            }

            else if (burst_counter == 6) {
                play_sound_effect("Explosion", 1, 0.9f);
                action_timer = 0.2;
            }
            // THE END
            else { 
                current_state = B_WALKING;
                state_timer = downtime + 1.0; 
            }
    }
        break;
    }
}

    update_animation(boss_anim);
}


void BossEnemy::apply_knockback(vector_2d force, double duration)
{
    (void)force;
    flash_timer = 0.25;
    stun_timer = 0.25;

    if (current_state == B_MELEE_COMBO) {
        velocity.x = 0;
        velocity.y = 0;
        return;
    }
   
}

int Enemy::get_id() const
{
    return id;
}

void Enemy::apply_knockback(vector_2d force, double duration)
{
    velocity = force;
    stun_timer = duration;
    flash_timer = 0.1;
}

KamikazeEnemy::KamikazeEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 40, 300) {}

void KamikazeEnemy::draw(bool show_debug)
{
    if (is_triggered)
    {
        double ratio = 1.0 - (fuse_timer / 1.5);
        draw_circle(rgba_color(255, 0, 0, 150), coordinates.x, coordinates.y, 100);

        double current_radius = 100 * ratio;
        fill_circle(rgba_color(255, 0, 0, 60), coordinates.x, coordinates.y, current_radius);

        double flash_rate = 0.05 + (fuse_timer * 0.1);
        draw_circle(rgba_color(255, 0, 0, 80), coordinates.x, coordinates.y, 100);
        if (std::fmod(fuse_timer, flash_rate * 2) > flash_rate)
        {
            fill_circle(COLOR_WHITE, coordinates, 15);
        }
        else
        {
            fill_circle(COLOR_RED, coordinates, 15);
        }
    }

    else
    {
        if (flash_timer > 0)
        {
            fill_circle(COLOR_WHITE, coordinates, 15);
        }

        else
        {
            fill_circle(COLOR_CYAN, coordinates, 15);
        }
    }

    Enemy::draw(show_debug);
}

void KamikazeEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles);
    // 1. THE SENSOR
    // If we haven't triggered yet, check the distance!
    if (!is_triggered)
    {
        double dist = point_point_distance(coordinates, target.get_coordinates());
        if (dist <= 150)
        {
            is_triggered = true; // ARM THE BOMB!
            movement_speed = 500;
            fuse_timer = 1; // Optional: Make it sprint faster when enraged!
        }
    }
    // 2. THE COUNTDOWN
    if (is_triggered)
    {
        fuse_timer -= dt;
        if (fuse_timer <= 0)
        {
            is_exploded = true; // Tell the engine to detonate!
            health = 0;         // Kill itself
            return;
        }
    }
    // 3. STUN CHECK
    if (stun_timer > 0)
        return;

    // 4. THE CHASE
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0)
    {
        vector_2d move_dir = unit_vector(raw_dir);
        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }
}

bool KamikazeEnemy::has_exploded() const
{
    return is_exploded;
}

bool BossEnemy::check_and_clear_aoe(double &out_radius)
{
    if (trigger_aoe) {
        trigger_aoe = false;
        out_radius = 200.0;
        return true;
    }
    return false;
}

double Enemy::get_collision_radius() const
{
    return 30.0;
}

double BossEnemy::get_collision_radius() const
{
    return 70.0;
}

double Enemy::get_damage_radius() const
{
    return 18.0;
}
double BossEnemy::get_damage_radius() const
{
    if (current_state == B_SHADOW_STEP) {
        if ((burst_counter == 2 || burst_counter == 4 || burst_counter == 6) && action_timer <= 0.15) {
            // The hitbox only exists for a tiny 0.1s window when the timer has just reset
            return 1000.0;
        }
        return -50.0;
    }

    if (current_state == B_LUNGE || current_state == B_MELEE_COMBO) {
        if (action_timer <= 0) {
            return 50.0;
        }
    }

    return -50.0;

}
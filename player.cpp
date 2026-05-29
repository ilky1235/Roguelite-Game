#include "player.h"
#include <cmath>

Player::Player(double x, double y)
    : Entity(x, y, 400, 450)
{
    dash_cooldown = 0;
    is_dashing = false;
    is_parrying = false;
    parry_timer = 0.15;
    parry_cooldown = 0.6;
    player_anim_script = animation_script_named("PlayerAnim");
    player_animation = create_animation(player_anim_script, "Idle");
    is_facing_left = false;
    current_animation = "Idle";
    arrow_ready = false;
    step_timer = 0;
    current_floor = FLOOR_GRASS;
}

void Player::handle_input(double dt)
{
    dash();

    vector_2d input_dir{};

    if (mouse_clicked(RIGHT_BUTTON))
    {
        attempt_parry();
    }

    if (parry_timer > 0)
    {
        parry_timer -= dt;
        if (parry_timer <= 0)
            is_parrying = false;
    }

    if (parry_cooldown > 0)
    {
        parry_cooldown -= dt;
    }

    if (key_down(D_KEY))
    {
        input_dir.x++;
        is_facing_left = false;
    }

    if (key_down(A_KEY))
    {
        input_dir.x--;
        is_facing_left = true;
    }

    if (key_down(W_KEY))
    {
        input_dir.y--;
    }

    if (key_down(S_KEY))
    {
        input_dir.y++;
    }

    if (vector_magnitude(input_dir) > 0)
    {
        input_dir = unit_vector(input_dir);
    }
    current_velocity.x = input_dir.x * movement_speed;
    current_velocity.y = input_dir.y * movement_speed;

    double dx = input_dir.x * movement_speed * dt;
    double dy = input_dir.y * movement_speed * dt;
    if (is_parrying)
    {
        dx *= 0.3;
        dy *= 0.3;
    }

    if (knockback_timer > 0)
    {
        knockback_timer -= dt;

        dx += knockback_vec.x * dt;
        dy += knockback_vec.y * dt;

        knockback_vec.x -= knockback_vec.x * 7 /*friction*/ * dt;
        knockback_vec.y -= knockback_vec.y * 7 * dt;
    }

    coordinates.x += dx;
    coordinates.y += dy;

    int border = 60;
    if (coordinates.x < border) {
        coordinates.x = border;
    } else if (coordinates.x > screen_width() - border -40) {
        coordinates.x = screen_width() - border - 40;
    }

    if (coordinates.y < border) {
        coordinates.y = border;
    } else if (coordinates.y > screen_height() - border + 20) {
        coordinates.y = screen_height() - border  + 20;
    }

    if (is_dashing)
    {

        dash_active_timer -= dt;
        if (dash_active_timer <= 0)
        {
            movement_speed /= 4;
            is_dashing = false;
            dash_cooldown = 0.5;
        }
    }

    if (combo_window_timer > 0) {
        combo_window_timer -= dt;
        if (combo_window_timer <= 0) {
            melee_combo_step = 0;
        }
    }
    if (dash_cooldown > 0)
    {
        dash_cooldown -= dt;
    }

    if (swing_timer > 0)
    {
        swing_timer -= dt;
    }

    else
    {
        is_swinging = false;
    }

    if (sword_cooldown > 0)
    {
        sword_cooldown -= dt;
    }

    if (i_frame_timer > 0)
    {
        i_frame_timer -= dt;
    }

    if (vector_magnitude(current_velocity) > 0 && !is_dashing) {
        step_timer -= dt;

        if (step_timer <= 0) {
            if (current_floor == FLOOR_GRASS) {
                play_sound_effect("StepGrass", 1, 0.5f);
            } else {
                play_sound_effect("StepStone", 1);
            }

            step_timer = 0.3;
        }
    } else {
        step_timer = 0;
    }

// ==========================================
    // --- SPRITE STATE MACHINE ---
    // ==========================================
    
    // 1. If Hurt, lock the animation until it finishes!
    if (current_animation == "Hurt" && i_frame_timer > 0.35)
    {
        // Do nothing! The animation script will automatically jump back to "Idle" when done.
    }
    // 2. If attacking, let the attack animation play!
    else if (is_swinging) {
        // Do nothing! The Player::attack() function already assigned 
    
    }
    // 3. If moving, play Run
    else if (vector_magnitude(current_velocity) > 0) {
        if (current_animation != "Run") {
            assign_animation(player_animation, player_anim_script, "Run");
            current_animation = "Run";
        }
    }
    // 4. Otherwise, Idle
    else {
        if (current_animation != "Idle") {
            assign_animation(player_animation, player_anim_script, "Idle");
            current_animation = "Idle";
        }
    }

    if (dt > 0){
        update_animation(player_animation);
    }
}

void Player::dash()
{
    if (key_typed(SPACE_KEY) && dash_cooldown <= 0 && !is_dashing)
    {
        play_sound_effect("Dash");
        movement_speed *= 4;
        is_dashing = true;
        dash_active_timer = 0.15;
        i_frame_timer = 0.25;

        just_dashed = true;
    }
}

bool Player::is_hitbox_active() const
{
    return is_swinging && (swing_timer <= 0.15) && (swing_timer >= 0.055);
}

void Player::attack(point_2d target)
{
    if (sword_cooldown <= 0)
    {
        hit_list.clear();
        sword_cooldown = 0.3;

        if (target.x < coordinates.x) is_facing_left = true;
        else is_facing_left = false;

        if (weapon == SWORD)
        {
            is_swinging = true;
            
            melee_combo_step++;
            if (melee_combo_step > 3) melee_combo_step = 1;
            combo_window_timer = 0.7;

            point_2d center = {coordinates.x, coordinates.y};
            
            // 360 degree
            vector_2d aim_dir = vector_point_to_point(center, target);
            if (vector_magnitude(aim_dir) > 0) {
                swing_dir = unit_vector(aim_dir);

            } else {
                swing_dir.x = is_facing_left ? -1 : 1;
                swing_dir.y = 0;
            }
           
            // 3 STEP COMBO LOGIC
            vector_2d lunge;
            double lunge_duration = 0.1;
            if (melee_combo_step == 1) {
                // Strike 1: Standard quick slash
                play_sound_effect("Sword");
                assign_animation(player_animation ,player_anim_script, "SwordAttack");
                current_animation = "SwordAttack";
                swing_timer = 0.25;
                lunge.x = swing_dir.x * 800;
                lunge.y = swing_dir.y * 800;
                combo_damage_mult = 1.0; // Base damage
                grant_iframes(0.25);
                lunge_duration = 0.12;
            } 
            else if (melee_combo_step == 2) {
                // Strike 2: Faster followup, pushes you slightly further
                play_sound_effect("Sword");
                assign_animation(player_animation ,player_anim_script, "SwordAttack");
                current_animation = "SwordAttack";
                swing_timer = 0.20; // Faster!
                lunge.x = swing_dir.x * 1000;
                lunge.y = swing_dir.y * 1000;
                grant_iframes(0.2);
                lunge_duration = 0.12;

            }
            else if (melee_combo_step == 3) {
                // Strike 3: HEAVY FINISHER!
                assign_animation(player_animation ,player_anim_script, "HeavyAttack"); // Or a special heavy animation!
                current_animation = "HeavyAttack";
                swing_timer = 0.35; // Slight delay for impact
                lunge.x = swing_dir.x * 1500; 
                lunge.y = swing_dir.y * 1500;
                combo_damage_mult = 2; 
                
                // Reset combo after the heavy finisher
                combo_window_timer = 0; 
                melee_combo_step = 0;
                grant_iframes(0.35);
                lunge_duration = 0.25;
                play_sound_effect("Combo", 1, 0.8f);
            }

            hitbox = {center.x + (swing_dir.x * 40), center.y + (swing_dir.y * 40)};
            apply_knockback(lunge, lunge_duration); 
        }

        else if (weapon == BOW)
        {
            assign_animation(player_animation, player_anim_script, "BowAttack");
            current_animation = "BowAttack";
            is_swinging = true;
            swing_timer = 0.4;
            arrow_ready = true;
            arrow_target = target;
        }
    }
}

void Player::draw_hitbox()
{
    if (is_hitbox_active())
    {
        fill_circle(rgba_color(100, 100, 100, 150), hitbox.x, hitbox.y, 45);
    }
}

// test
bool Player::get_is_swinging() const
{
    return is_swinging;
}

point_2d Player::get_attack_hitbox() const
{
    return hitbox;
}

bool Player::has_hit(int enemy_id)
{
    for (int i = 0; i < hit_list.length(); i++)
    {
        if (enemy_id == hit_list[i])
        {
            return true;
        }
    }

    return false;
}

void Player::add_to_hitlist(int enemy_id)
{
    hit_list.add(enemy_id);
}

bool Player::is_dead() const
{
    return health <= 0;
}

void Player::attempt_parry()
{
    // You can only parry if you aren't already swinging, parrying, or recovering!
    if (parry_cooldown <= 0 && !get_is_swinging())
    {
        is_parrying = true;
        parry_timer = 0.22;   // The strict 220 God-Mode window
        parry_cooldown = 0.3; // The half-second punishment if you miss
        parry_landed = false;
    }
}

void Player::draw()
{
    if (is_swinging) {
        if (swing_timer <= 0.15 && swing_timer >= 0.05) {
            // Calculate how far along the swing we are
            double time_passed = 0.15 - swing_timer;
            double progress = time_passed / 0.095;

            // Expand slash radius
            double current_radius = 20 + (progress * 25);
            int alpha = 255 - int(progress * 255);
            if (alpha < 0) alpha = 0;

            color trail_col = rgba_color(200, 220, 255, alpha); // A cool steel-blue tint
            color glow_col = rgba_color(200, 255, 255, alpha);

            if (combo_damage_mult > 2.0) {
                current_radius = 35 + (progress * 45);
                trail_col = rgba_color(255, 50, 50, alpha);
                glow_col = rgba_color(255, 100, 0, alpha);
            }

            fill_circle(glow_col, hitbox.x, hitbox.y, current_radius);
            fill_circle(rgba_color(255, 255, 255, alpha), hitbox.x, hitbox.y, current_radius / 1.5);
            draw_circle(trail_col, hitbox.x, hitbox.y, current_radius - 1);
            draw_circle(trail_col, hitbox.x, hitbox.y, current_radius - 2);
        }
    }
    if (is_parrying)
    {
        double time_left = parry_timer / 0.22;

        if (time_left < 0)
        {
            time_left = 0;
        }
        double time_passed = 1 - time_left;

        // Shrinking radius
        double wave_radius = 20 + (time_passed * 45);
        int alpha = int (255 * time_left);
        if (alpha < 0) alpha = 0;

        color wave_col = rgba_color(0, 255, 255, alpha);

    

        // SplashKit lines are 1 pixel thin, so we draw 3 to make a THICK white ring!
        draw_circle(wave_col, coordinates.x, coordinates.y, wave_radius);
        draw_circle(wave_col, coordinates.x, coordinates.y, wave_radius - 1);
        draw_circle(wave_col, coordinates.x, coordinates.y, wave_radius - 2);

        //  The Core Energy Shield
        // A solid cyan edge with a white inner core so it pops!
        draw_circle(rgba_color(0, 255, 255, 255), coordinates.x, coordinates.y, 45);
        draw_circle(COLOR_WHITE, coordinates.x, coordinates.y, 44);

        // A low-opacity filled circle to literally tint the background blue inside the shield
        fill_circle(rgba_color(0, 200, 255, 60), coordinates.x, coordinates.y, 45);
    }

    drawing_options opts = option_with_animation(player_animation);

    if (is_facing_left)
    {
        opts.flip_y = true;
    }

    opts.scale_x = 2.0;
    opts.scale_y = 2.0;

    draw_bitmap(bitmap_named("Player"), coordinates.x - 60, coordinates.y - 70, opts);


}

bool Player::is_invincible() const
{
    return i_frame_timer > 0;
}

void Player::take_damage(double amount)
{
    if (!is_invincible())
    {
        health -= amount;
        play_sound_effect("Hurt", 1, 0.8f);

        i_frame_timer = 0.5;
        assign_animation(player_animation, player_anim_script, "Hurt");
        current_animation = "Hurt";

        is_swinging = false;
    }
}

void Player::apply_knockback(vector_2d force, double time)
{
    knockback_vec = force;
    knockback_timer = time;
}

void Player::set_coordinates(double new_x, double new_y)
{
    coordinates.x = new_x;
    coordinates.y = new_y;
}

void Player::increase_damage(double amount)
{
    sword_damage += amount;
}

void Player::increase_speed(double amount)
{
    movement_speed += amount;
}

void Player::heal(double amount)
{
    health += amount;
    if (health > max_health)
    {
        health = max_health;
    }
}

double Player::get_sword_damage()
{
    return sword_damage * combo_damage_mult;
}

void Player::reset_stats()
{
    coordinates.x = 640;
    coordinates.y = 360;
    health = max_health;
    sword_damage = 25; 
    movement_speed = 400;
    hit_list.clear();
}

vector_2d Player::get_curr_velocity() const
{
    return current_velocity;
}

bool Player::get_is_parrying() const
{
    return is_parrying;
}

void Player::equip_weapon(WeaponType w)
{
    weapon = w;
}

WeaponType Player::get_weapon() const
{
    return weapon;
}

point_2d Player::get_arrow_target() const
{
    return arrow_target;
}

bool Player::pull_trigger()
{
    if (arrow_ready)
    {
        arrow_ready = false;
        return true;
    }

    return false;
}

void Player::draw_hud()
{
    double hud_x = camera_x() + 30;
    double hud_y = camera_y() + screen_height() - 50;

    double hp_percent= health / max_health;
    if (hp_percent < 0) hp_percent = 0;

    color hp_color = COLOR_GREEN;
    if (hp_percent <= 0.2) hp_color = COLOR_RED;



    // A. The Solid Base: Fully opaque (255) slate-black to block out the world behind it
    fill_rectangle(rgba_color(15, 15, 20, 255), hud_x, hud_y, 220, 40);

    // B. The Borders: A crisp white outer ring and a grey inner ring to make it pop
    draw_rectangle(COLOR_WHITE, hud_x, hud_y, 220, 40);
    draw_rectangle(COLOR_GRAY, hud_x + 2, hud_y + 2, 216, 36);

    
    // Draw the green health bar safely nested inside the borders
    fill_rectangle(hp_color, hud_x + 10, hud_y + 10, 200 * hp_percent, 20);

    // Draw the text
    string hp_text = std::to_string((int)health) + " / " + std::to_string((int)max_health);
    draw_text(hp_text, COLOR_WHITE, "TextFont", 16, hud_x + 15, hud_y + 11);
}

void Player::add_boon(Boon b)
{
    active_boons.add(b);

    if (b.id == BOON_ARES_STRIKE) {
        sword_damage += 40;
    } else if (b.id == BOON_ARES_SWIFTNESS) {
        movement_speed += 150;
    } else if (b.id == BOON_ZEUS_STRIKE) {
        sword_damage += 15;
    } else if (b.id == BOON_ZEUS_SWIFTNESS) {
        movement_speed += 200;
    }
}

bool Player::has_boon(BoonID target_id) 
{
    for (int i = 0; i < active_boons.length(); i++) {
        if (active_boons[i].id == target_id) {
            return true;
        }
    }
    return false;
}

void Player::set_floor_type(FloorType f)
{
    current_floor = f;
}

bool Player::pull_trigger(point_2d &target_out)
{
    if (arrow_ready) {
        target_out = arrow_target; // Pass the mouse coordinates out
        arrow_ready = false;       // Reset the chamber!
        return true;
    }
    return false;
}

void Player::grant_iframes(double duration)
{
    if (i_frame_timer < duration) {
        i_frame_timer = duration;
    }
}

void Player::reset_parry_cooldown()
{
    parry_cooldown = 0.1;
    is_parrying = false;
}

double Player::get_combo_mult() const
{
    return combo_damage_mult;
}

bool Player::get_and_clear_dash_event()
{
    if (just_dashed) {
        just_dashed = false;
        return true;
    }
    return false;
}


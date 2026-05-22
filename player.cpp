#include "player.h"

Player::Player(double x, double y)
    : Entity(x, y, 400, 400)
{
    dash_cooldown = 0;
    is_dashing = false;
    is_parrying = false;
    parry_timer = 0.15;
    parry_cooldown = 0.6;
}

void Player::handle_input(double dt) 
{
    dash();

    vector_2d input_dir{};

    if (mouse_clicked(RIGHT_BUTTON)) {
        attempt_parry();
    }

    if (parry_timer > 0) {
        parry_timer -= dt;
        if (parry_timer <= 0) is_parrying = false;
    }

    if (parry_cooldown > 0) {
        parry_cooldown -= dt;
    }
    
    if (key_down(D_KEY)) {
        input_dir.x++;
    }

    if (key_down(A_KEY)) {
        input_dir.x--;
    }

    if (key_down(W_KEY)) {
        input_dir.y--;
    }

    if (key_down(S_KEY)) {
        input_dir.y++;
    }

    if (vector_magnitude(input_dir) > 0) {
        input_dir = unit_vector(input_dir);
    }
    current_velocity.x = input_dir.x * movement_speed;
    current_velocity.y = input_dir.y * movement_speed;

    double dx = input_dir.x * movement_speed * dt;
    double dy = input_dir.y * movement_speed * dt;

    if (knockback_timer > 0) {
        knockback_timer -= dt;

        dx += knockback_vec.x * dt;
        dy += knockback_vec.y * dt;

        knockback_vec.x -= knockback_vec.x * 7 /*friction*/ * dt;
        knockback_vec.y -= knockback_vec.y * 7 * dt;
    }

    coordinates.x += dx;
    coordinates.y += dy;

    if (coordinates.x < 0) {
        coordinates.x = 0;
    }

    else if (coordinates.x + 40 > screen_width()) {
        coordinates.x = screen_width() - 40;
    }

    if (coordinates.y < 0) {
        coordinates.y = 0;
    }

    else if(coordinates.y + 40 > screen_height()) {
        coordinates.y = screen_height() - 40;
    }

    if (is_dashing) {

        dash_active_timer -= dt;
        if (dash_active_timer <= 0) {
            movement_speed /= 4;
            is_dashing = false;
            dash_cooldown = 0.5;
        }
    }

    if (dash_cooldown > 0) {
        dash_cooldown -= dt;
    }

    if (swing_timer > 0) {
        swing_timer -= dt;
    }

    else {
        is_swinging = false;
    }

    if (sword_cooldown > 0) {
        sword_cooldown -= dt;
    }

    if (i_frame_timer > 0) {
        i_frame_timer -= dt;
    }
}

void Player::dash()
{
    if (key_typed(SPACE_KEY) && dash_cooldown <= 0) {
        movement_speed *= 4;
        is_dashing = true;
        dash_active_timer = 0.15;
        i_frame_timer = 0.1;
    }
}

void Player::attack(point_2d target)
{
    if (sword_cooldown <= 0) {
        hit_list.clear();
        is_swinging = true;
        swing_timer = 0.15; // Swing lasts for 0.15 seconds
        sword_cooldown = 0.4;

        point_2d center = {coordinates.x, coordinates.y};

        vector_2d raw_vector = vector_point_to_point(center, target);
        if (vector_magnitude(raw_vector) > 0) {
            swing_dir = unit_vector(raw_vector);
        } else {
            swing_dir.x = 1;
            swing_dir.y = 0;
        }

        hitbox = {center.x + (swing_dir.x * 40), center.y + (swing_dir.y * 40)};
    }
}

void Player::draw_hitbox()
{
    if (is_swinging) {
        fill_circle(rgba_color(100, 100, 100, 150), hitbox.x, hitbox.y, 45);
    }
}

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
    for (int i = 0; i < hit_list.length(); i++) {
        if (enemy_id == hit_list[i]) {
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

void Player::attempt_parry() {
    // You can only parry if you aren't already swinging, parrying, or recovering!
    if (parry_cooldown <= 0 && !get_is_swinging()) {
        is_parrying = true;
        parry_timer = 0.15;   // The strict 150ms God-Mode window
        parry_cooldown = 0.6; // The half-second punishment if you miss
    }
}

void Player::draw() 
{
    if (is_parrying) {
        draw_circle(COLOR_WHITE, coordinates.x, coordinates.y, 45);
        draw_circle(COLOR_CYAN, coordinates.x, coordinates.y, 40);
    }

    if (is_invincible()) {
        fill_circle(rgba_color(0, 0, 0, 100), coordinates, 20);

    } else {
        fill_circle(COLOR_BLACK, coordinates, 20);
    }
    double hp_percent = health / max_health;

    if (hp_percent < 0) {
        hp_percent = 0;
    }

    fill_rectangle(COLOR_BLACK, coordinates.x - 20, coordinates.y - 30, 40, 5);
    fill_rectangle(COLOR_GREEN, coordinates.x - 20, coordinates.y - 30, 40 * hp_percent , 5);
}

bool Player::is_invincible() const
{
    return i_frame_timer > 0;
}

void Player::take_damage(double amount)
{
    if (!is_invincible()) {
        health -= amount;

        i_frame_timer = 0.5;
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
    if (health > max_health) {
        health = max_health;
    }
}

double Player::get_sword_damage()
{
    return sword_damage;
}

void Player::reset_stats() {
    coordinates.x = 640;
    coordinates.y = 360;
    health = max_health;
    sword_damage = 25; // Or whatever your base damage is
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




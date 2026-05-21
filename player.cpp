#include "player.h"

Player::Player(double x, double y)
    : Entity(x, y, 400, 400)
{
    dash_cooldown = 0;
    is_dashing = false;
    dash_timer = create_timer("Dash Timer");
}

void Player::handle_input(double dt, dynamic_array<rectangle> rectangles) 
{
    dash();

    vector_2d input_dir{};
    
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

    double dx = input_dir.x * movement_speed * dt;
    double dy = input_dir.y * movement_speed * dt;

    // Axis Split
    double old_x = coordinates.x;
    coordinates.x += dx;

    rectangle player_box = rectangle_from(coordinates.x-20, coordinates.y-20, 40, 40);

    for (int i = 0; i < rectangles.length(); i++) {
        if (rectangles_intersect(player_box, rectangles[i])) {
            coordinates.x= old_x;
            break;
        }
    }

    double old_y = coordinates.y;
    coordinates.y += dy;

    player_box = rectangle_from(coordinates.x-20, coordinates.y-20, 40, 40);

    for (int i = 0; i < rectangles.length(); ++i) {
        if (rectangles_intersect(player_box, rectangles[i])) {
            coordinates.y = old_y;
            break;
        }
    }

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

        if (timer_ticks(dash_timer) >= 150) {
            movement_speed /= 4;
            is_dashing = false;
            dash_cooldown = 1;
            reset_timer(dash_timer);

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
        start_timer(dash_timer);
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

void Player::draw() 
{
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


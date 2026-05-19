#include "entity.h"
#include <cmath>
#include "dynamic_array.h"


Entity::Entity(double x, double y, double health, double speed)
{
    coordinates.x = x;
    coordinates.y = y;
    this->health = health;
    movement_speed = speed;
}

Player::Player(double x, double y)
    : Entity(x, y, 100, 400)
{
    dash_cooldown = 0;
    is_dashing = false;
    dash_timer = create_timer("Dash Timer");
}

Enemy::Enemy(double x, double y)
    : Entity(x, y, 80, 250)
{

}

void Entity::draw()
{
    fill_rectangle(COLOR_BLACK, coordinates.x, coordinates.y, 40, 40);
}

void Entity::take_damage(double amount)
{
    health -= amount;
}

void Entity::update()
{

}

point_2d Entity::get_coordinates() const
{
    return coordinates;
}

void Player::handle_input(double dt) 
{
    dash();
    if (key_down(D_KEY)) {
        coordinates.x += movement_speed * dt;
    }

    if (key_down(A_KEY)) {
        coordinates.x -= movement_speed * dt;
    }

    if (key_down(W_KEY)) {
        coordinates.y -= movement_speed * dt;
    }

    if (key_down(S_KEY)) {
        coordinates.y += movement_speed * dt;
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
            dash_cooldown = 2;
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




}

void Player::dash()
{
    if (key_typed(SPACE_KEY) && dash_cooldown <= 0) {
        movement_speed *= 4;
        is_dashing = true;
        start_timer(dash_timer);
    }
}

void Player::attack(point_2d target)
{
    if (sword_cooldown <= 0) {
        is_swinging = true;
        swing_timer = 0.15; // Swing lasts for 0.15 seconds
        sword_cooldown = 0.4;

        point_2d center = {coordinates.x + 20, coordinates.y + 20};

        vector_2d raw_vector = vector_point_to_point(center, target);
        if (vector_magnitude(raw_vector) > 0) {
            swing_dir = unit_vector(raw_vector);
        } else {
            swing_dir.x = 1;
            swing_dir.y = 0;
        }
    }
}

void Player::draw_hitbox()
{
    if (is_swinging) {
        point_2d center = {coordinates.x + 20, coordinates.y + 20};
        hitbox = {center.x + (swing_dir.x * 40), center.y + (swing_dir.y * 40)};

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

void Enemy::draw()
{
    fill_circle(COLOR_RED, coordinates, 5);
}

void Enemy::update(const Player &target, double dt, dynamic_array <Projectile> &projectiles)
{
    double target_center_x = target.get_coordinates().x + 20;
    double target_center_y = target.get_coordinates().y + 20;
    double dx = target_center_x - coordinates.x;
    double dy = target_center_y - coordinates.y;

    double magnitude = std::sqrt((float)((dx * dx) + (dy * dy)));

    if (magnitude > 0) {
        coordinates.x += (dx/magnitude) * movement_speed * dt;
        coordinates.y += (dy / magnitude ) * movement_speed * dt;
    }

    if (reload_timer <= 0) {
        Projectile new_projectile(coordinates, target.get_coordinates());
        projectiles.add(new_projectile);
        reload_timer = 1;
    }

    else {
        reload_timer -= dt;
    }
}

Projectile::Projectile(point_2d start, point_2d target)
    : Entity(start.x, start.y, 1, 700)
{
    double target_center_x = target.x + 20;
    double target_center_y = target.y + 20;
    double dx = target_center_x - coordinates.x;
    double dy = target_center_y - coordinates.y;   

    double magnitude = std::sqrt((float)((dx * dx) + (dy * dy)));

    if (magnitude > 0) {
        vel_x = (dx/magnitude) * movement_speed;
        vel_y = (dy/magnitude) * movement_speed;
    }
    
}

void Projectile::update(double dt)
{
    coordinates.x += vel_x * dt;
    coordinates.y += vel_y * dt;
}

void Projectile::draw()
{
    fill_rectangle(COLOR_ORANGE, coordinates.x, coordinates.y, 10, 10);
}


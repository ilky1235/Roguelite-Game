#include "enemy.h"

Enemy::Enemy(double x, double y, int assigned_id)
    : Entity(x, y, 80, 250)
{
    id = assigned_id;
}

void Enemy::draw()
{
    fill_circle(COLOR_RED, coordinates.x, coordinates.y,  20);

    double hp_percent = health / max_health;

    if (hp_percent < 0) {
        hp_percent = 0;
    }

    fill_rectangle(COLOR_BLACK, coordinates.x - 20, coordinates.y - 30, 40, 5);
    fill_rectangle(COLOR_GREEN, coordinates.x - 20, coordinates.y - 30, 40 * hp_percent , 5);
}

void Enemy::update(const Player &target, double dt, dynamic_array <Projectile> &projectiles)
{
    double target_center_x = target.get_coordinates().x;
    double target_center_y = target.get_coordinates().y;
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

int Enemy::get_id() const
{
    return id;
}
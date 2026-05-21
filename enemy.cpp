#include "enemy.h"
#include <cmath>

Enemy::Enemy(double x, double y, int assigned_id, bool melee_type)
    : Entity(x, y, 80, 250)
{
    id = assigned_id;
    is_melee = melee_type;
    reload_timer = 1;
}

void Enemy::draw()
{
    if (flash_timer > 0) {
        fill_circle(COLOR_WHITE, coordinates, 20);
    } else if (is_melee){
        fill_circle(COLOR_BLUE, coordinates, 20);
    } else {
        fill_circle(COLOR_RED, coordinates, 20);
    }

    if (is_winding_up) {
        double flash_rate = 0.1 - (0.07 * (1 - windup_timer / 0.3));
        if (std::fmod(windup_timer, flash_rate * 2) < flash_rate) {
            draw_text("!", COLOR_ORANGE, "TitleFont", 24, coordinates.x - 5, coordinates.y - 45);
        }
   
    }

    double hp_percent = health / max_health;

    if (hp_percent < 0) {
        hp_percent = 0;
    }

    fill_rectangle(COLOR_BLACK, coordinates.x - 20, coordinates.y - 20, 40, 5);
    fill_rectangle(COLOR_GREEN, coordinates.x-20, coordinates.y-20, 40 * hp_percent, 5);
}

void Enemy::update(const Player &target, double dt, dynamic_array <Projectile> &projectiles, dynamic_array <rectangle> &pillars)
{
    // Part 1: Physics
    coordinates.x += velocity.x * dt;
    coordinates.y += velocity.y * dt;

    velocity.x -= velocity.x *8.0 * dt;
    velocity.y -= velocity.y *8.0 * dt;


    if (flash_timer > 0) {
        flash_timer -= dt;
    }

    if (stun_timer > 0) {
        stun_timer -=dt;
        return;
    }

    // Part 2: AI
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());

    if (vector_magnitude(raw_dir) > 0) {
        vector_2d move_dir = unit_vector(raw_dir); // Normalize it!
        
        double dx = move_dir.x *movement_speed *dt;
        double dy = move_dir.y * movement_speed * dt;

        double old_x = coordinates.x;
        coordinates.x += dx;

        rectangle enemy_box = rectangle_from(coordinates.x - 20, coordinates.y - 20, 40, 40);
        for (int i = 0; i < pillars.length(); i++) {
            if (rectangles_intersect(enemy_box, pillars[i])) {
                coordinates.x = old_x;
                break;
            }
        }

        double old_y = coordinates.y;
        coordinates.y += dy;
        enemy_box = rectangle_from(coordinates.x - 20, coordinates.y - 20, 40, 40);
        for (int i = 0; i < pillars.length(); i++) {
            if (rectangles_intersect(enemy_box, pillars[i])) {
                coordinates.y = old_y;
                break;
            }
        }
    }

    if (is_melee) {
        return;
    }

    if (reload_timer <= 0) {
        if (!is_winding_up) {
            is_winding_up = true;
            windup_timer = 0.3;
        } else {
            windup_timer -= dt;
            if (windup_timer <= 0) {
                Projectile new_projectile(coordinates, target.get_coordinates());
                projectiles.add(new_projectile);
                reload_timer = 1;
                is_winding_up = false;
            }
        }
    } else {
        reload_timer -= dt;
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
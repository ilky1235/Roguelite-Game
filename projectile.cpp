#include "projectile.h"

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


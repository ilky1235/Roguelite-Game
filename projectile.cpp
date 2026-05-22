#include "projectile.h"

Projectile::Projectile(point_2d start, point_2d target)
    : Entity(start.x, start.y, 1, 700)
{
    // NO MORE + 20 AND NO MORE MANUAL MATH!
    vector_2d raw_dir = vector_point_to_point(coordinates, target);

    if (vector_magnitude(raw_dir) > 0) {
        vector_2d move_dir = unit_vector(raw_dir);
        
        velocity.x = move_dir.x * movement_speed;
        velocity.y = move_dir.y * movement_speed;
    } else {
        velocity.x = 0;
        velocity.y = 0;
    }
}

void Projectile::update(double dt)
{
    coordinates.x += velocity.x * dt;
    coordinates.y += velocity.y * dt;
}

void Projectile::draw()
{
    fill_rectangle(COLOR_ORANGE, coordinates.x, coordinates.y, 10, 10);
}

bool Projectile::get_is_deflected() const
{
    return is_deflected;
}

void Projectile::deflect()
{
    is_deflected = true;
    velocity.x *= -1.5;
    velocity.y *= -1.5;
}


#include "projectile.h"

Projectile::Projectile(point_2d start, point_2d target)
    : Entity(start.x, start.y, 1, 700)
{
    is_friendly = false;
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

bool Projectile::get_is_friendly() const
{
    return is_friendly;
}

void Projectile::set_is_friendly(bool friendly)
{
    is_friendly = friendly;
}

void Projectile::update(double dt)
{
    coordinates.x += velocity.x * dt;
    coordinates.y += velocity.y * dt;

    if (!is_friendly) {
        anim_timer += dt;
        if (anim_timer >= 0.1) {
            current_frame++;
            if (current_frame > 2) {
                current_frame = 0;
            }
            anim_timer = 0;
        }
    }
}

void Projectile::draw()
{
    double angle = vector_angle(velocity);
    
    if (is_friendly) {
        // Arrow code
        bitmap arrow_bmp = bitmap_named("ArrowSprite");
        drawing_options opts = option_defaults();
        opts.angle = angle;

        double draw_x = coordinates.x - (bitmap_width(arrow_bmp) / 2);
        double draw_y = coordinates.y - (bitmap_width(arrow_bmp) / 2);

        draw_bitmap(arrow_bmp, draw_x, draw_y, opts);
    } else {
        bitmap enemy_shot_bmp = bitmap_named("EnemyShotSprite");

        drawing_options opts = option_with_bitmap_cell(current_frame);
        opts.angle = angle;
        opts.scale_x = 2.0;
        opts.scale_y = 2.0;

        // Create the draw coordinates using cell width
        double draw_x = coordinates.x - (bitmap_cell_width(enemy_shot_bmp) / 2.0);
        double draw_y = coordinates.y - (bitmap_cell_width(enemy_shot_bmp) / 2.0);

        if (is_deflected) {
            fill_circle(COLOR_WHITE, coordinates, 15);
        }

        draw_bitmap(enemy_shot_bmp, draw_x, draw_y, opts);
    }



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




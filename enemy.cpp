#include "enemy.h"
#include <cmath>

Enemy::Enemy(double x, double y, int assigned_id, double health, double speed)
    : Entity(x, y, health, speed)
{
    id = assigned_id;
    reload_timer = 1;
}

Enemy::~Enemy(){}

void Enemy::draw()
{

    double hp_percent = health / max_health;

    if (hp_percent < 0) {
        hp_percent = 0;
    }

    fill_rectangle(COLOR_BLACK, coordinates.x - 20, coordinates.y - 20, 40, 5);
    fill_rectangle(COLOR_GREEN, coordinates.x-20, coordinates.y-20, 40 * hp_percent, 5);
}

void Enemy::update(const Player &target, double dt, dynamic_array <Projectile> &projectiles)
{
    (void) target;
    (void) projectiles;

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
    }

}

ShooterEnemy::ShooterEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 80, 250) {

}

void ShooterEnemy::draw()
{
    if (flash_timer > 0) {
        fill_circle(COLOR_WHITE, coordinates, 20);
    } else {
        fill_circle(COLOR_RED, coordinates, 20);
    }

    if (is_winding_up) {
        double flash_rate = 0.1 - (0.07 * (1 - windup_timer / 0.3));
        if (std::fmod(windup_timer, flash_rate * 2) < flash_rate) {
            draw_text("!", COLOR_ORANGE, "TitleFont", 24, coordinates.x - 5, coordinates.y - 45);
        }
    }

    Enemy::draw();
}

void ShooterEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles) {
    Enemy::update(target, dt, projectiles); 
    if (stun_timer > 0) {
        return; 
    }

    // AI: Walk to player
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0) {
        vector_2d move_dir = unit_vector(raw_dir);
        
        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }

    // AI: Shooting Logic
    if (reload_timer <= 0) {
        if (!is_winding_up) {
            is_winding_up = true;
            windup_timer = 0.3;
        } else {
            windup_timer -= dt;
            if (windup_timer <= 0) {
                projectiles.add(Projectile(coordinates, target.get_coordinates()));
                reload_timer = 1;
                is_winding_up = false;
            }
        }
    } else { reload_timer -= dt; }
}

ChaserEnemy::ChaserEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 80, 400) {}

void ChaserEnemy::draw() {
    if (flash_timer > 0) fill_circle(COLOR_WHITE, coordinates, 20);
    else fill_circle(COLOR_BLUE, coordinates, 20);
    
    Enemy::draw(); // Draw the health bar!
}

void ChaserEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles) {
    Enemy::update(target, dt, projectiles); // Run Base Physics
    if (stun_timer > 0) return; // Abort AI if stunned!

    // AI: Sprint at player
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0) {
        vector_2d move_dir = unit_vector(raw_dir);
        
        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }
}

BossEnemy::BossEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 1000, 150)
{
    current_state = B_WALKING;
    state_timer = 2.0;
    action_timer = 0;
    burst_counter = 0;
}

void BossEnemy::draw() 
{
    // Draw the Telegraphs based on the State!
    if (current_state == B_TRIPLE_BURST) {
        draw_text("TRIPLE BURST!", COLOR_RED, "TitleFont", 20, coordinates.x - 50, coordinates.y - 60);
        fill_circle(COLOR_RED, coordinates, 40);
    } 
    else if (current_state == B_SHOTGUN) {
        draw_text("SHOTGUN!", COLOR_ORANGE, "TitleFont", 20, coordinates.x - 40, coordinates.y - 60);
        fill_circle(COLOR_ORANGE, coordinates, 40);
    } 
    else if (current_state == B_LUNGE) {
        draw_text("LUNGE!", COLOR_YELLOW, "TitleFont", 20, coordinates.x - 30, coordinates.y - 60);
        if (action_timer > 0) { // Flashing white during telegraph
            fill_circle(COLOR_WHITE, coordinates, 40);
        } else {
            fill_circle(COLOR_YELLOW, coordinates, 40);
        }
    } 
    else {
        if (flash_timer > 0) fill_circle(COLOR_WHITE, coordinates, 40);
        else fill_circle(COLOR_PURPLE, coordinates, 40);
    }

    // Custom Massive Health Bar
    double hp_percent = health / max_health;
    if (hp_percent < 0) hp_percent = 0;
    fill_rectangle(COLOR_BLACK, coordinates.x - 40, coordinates.y - 50, 80, 8);
    fill_rectangle(COLOR_RED, coordinates.x - 40, coordinates.y - 50, 80 * hp_percent, 8);
}

void BossEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles) 
{
    Enemy::update(target, dt, projectiles); 
    if (stun_timer > 0) return; 

    // ==========================================
    // --- THE FINITE STATE MACHINE (FSM) ---
    // ==========================================
    switch(current_state) {

        case B_WALKING: {
            // Simply walk toward the player
            vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
            if (vector_magnitude(raw_dir) > 0) {
                vector_2d move_dir = unit_vector(raw_dir);
                coordinates.x += move_dir.x * movement_speed * dt;
                coordinates.y += move_dir.y * movement_speed * dt;
            }

            // Count down to the next attack
            state_timer -= dt;
            if (state_timer <= 0) {
                // Roll a 3-sided dice to pick a random attack
                float roll = rnd();
                if (roll < 0.33) {
                    current_state = B_TRIPLE_BURST;
                    action_timer = 0.5; // 0.5s Telegraph
                    burst_counter = 0;
                } else if (roll < 0.66) {
                    current_state = B_SHOTGUN;
                    action_timer = 0.6; // 0.6s Telegraph
                } else {
                    current_state = B_LUNGE;
                    action_timer = 0.8; // 0.8s Telegraph
                }
            }
            break;
        }

        case B_TRIPLE_BURST: {
            action_timer -= dt;
            if (action_timer <= 0) {
                
                // Fire a ring of 12 projectiles
                for (int i = 0; i < 12; i++) {
                    // Offset the angle slightly for each burst wave (burst_counter * 15 degrees)
                    double angle = (i * 30) + (burst_counter * 15);
                    
                    point_2d proj_target;
                    proj_target.x = coordinates.x + cosine(angle) * 100;
                    proj_target.y = coordinates.y + sine(angle) * 100;
                    projectiles.add(Projectile(coordinates, proj_target));
                }

                burst_counter++;
                if (burst_counter >= 3) {
                    // Finished firing 3 waves, go back to walking!
                    current_state = B_WALKING;
                    state_timer = 2.0; 
                } else {
                    // Wait 0.3 seconds before firing the next ring
                    action_timer = 0.3; 
                }
            }
            break;
        }

        case B_SHOTGUN: {
            action_timer -= dt;
            if (action_timer <= 0) {
                
                // 1. Find the exact angle to the player
                vector_2d dir = vector_point_to_point(coordinates, target.get_coordinates());
                double base_angle = vector_angle(dir);

                // 2. Fire 7 bullets in a cone spread
                for (int i = -3; i <= 3; i++) {
                    double angle = base_angle + (i * 10); // Spread bullets 10 degrees apart
                    
                    point_2d proj_target;
                    proj_target.x = coordinates.x + cosine(angle) * 100;
                    proj_target.y = coordinates.y + sine(angle) * 100;
                    projectiles.add(Projectile(coordinates, proj_target));
                }

                current_state = B_WALKING;
                state_timer = 1.5;
            }
            break;
        }

        case B_LUNGE: {
            if (action_timer > 0) {
                // Phase 1: Telegraphing (Standing perfectly still)
                action_timer -= dt;

                if (action_timer >= 0.2) {
                    point_2d p_pos = target.get_coordinates();
                    vector_2d p_vel = target.get_curr_velocity();

                    locked_target.x = p_pos.x + (p_vel.x * 0.9);
                    locked_target.y = p_pos.y + (p_vel.y * 0.9);
                }
                

                if (action_timer <= 0) {
                    vector_2d raw_dir = vector_point_to_point(coordinates, locked_target);
                    if (vector_magnitude(raw_dir) > 0) {
                        raw_dir = unit_vector(raw_dir);

                        velocity.x = raw_dir.x * 2500;
                        velocity.y = raw_dir.y * 2500;
                    }
                    state_timer = 0.6;
                }
            } else {
                state_timer -= dt;
                if (state_timer <= 0) {
                    current_state = B_WALKING;
                    state_timer = 2.0;
                }
            }
            break;
        }
    }
}


void BossEnemy::apply_knockback(vector_2d force, double duration)
{
    (void) force;
    (void) duration;

    stun_timer = duration;
    flash_timer = 0.1;
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

void KamikazeEnemy::draw()
{
    if (is_triggered) {
        double ratio= 1.0 - (fuse_timer / 1.5);
        draw_circle(rgba_color(255, 0, 0, 150), coordinates.x, coordinates.y, 100);

        double current_radius = 100 * ratio;
        fill_circle(rgba_color(255, 0, 0, 60), coordinates.x, coordinates.y, current_radius);

        double flash_rate = 0.05 + (fuse_timer * 0.1);
        draw_circle(rgba_color(255, 0, 0, 80), coordinates.x, coordinates.y, 100);
        if (std::fmod(fuse_timer , flash_rate * 2) > flash_rate) {
            fill_circle(COLOR_WHITE, coordinates, 15);
           
        } else {
            fill_circle(COLOR_RED, coordinates, 15);
        }
    }

    else {
        if (flash_timer > 0) {
            fill_circle(COLOR_WHITE, coordinates, 15);
        }

        else {
            fill_circle(COLOR_CYAN, coordinates, 15);
        }
    }

    
    Enemy::draw();
}

void KamikazeEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles) {
    Enemy::update(target, dt, projectiles); 
    // 1. THE SENSOR
    // If we haven't triggered yet, check the distance!
    if (!is_triggered) {
        double dist = point_point_distance(coordinates, target.get_coordinates());
        if (dist <= 60) {
            is_triggered = true;   // ARM THE BOMB!
            movement_speed = 520;
            fuse_timer = 1.5;  // Optional: Make it sprint faster when enraged!
        }
    }
    // 2. THE COUNTDOWN
    if (is_triggered) {
        fuse_timer -= dt;
        if (fuse_timer <= 0) {
            is_exploded = true; // Tell the engine to detonate!
            health = 0;         // Kill itself
            return;
        }
    }
    // 3. STUN CHECK
    if (stun_timer > 0) return; 

    // 4. THE CHASE
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0) {
        vector_2d move_dir = unit_vector(raw_dir);
        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }
}

bool KamikazeEnemy::has_exploded() const 
{
    return is_exploded;
}
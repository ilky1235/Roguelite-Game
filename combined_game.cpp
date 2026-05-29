// Combined SplashKit Game File


#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <string>
#include <stdlib.h>
#include "splashkit.h"

class array_full
{
};

template <typename T>

class dynamic_array
{
    int size;
    int capacity;
    T *data;

public:
    dynamic_array(void)
    {
        size = 0;
        capacity = 4;
        data = (T *)malloc(sizeof(T) * capacity);
    }

    ~dynamic_array(void)
    {
        for (int i = 0; i < size; i++)
        {
            data[i].~T();
        }

        free(data);
    }

    dynamic_array(const dynamic_array &other)
    {
        size = other.size;
        capacity = other.capacity;
        data = (T *)malloc(sizeof(T) * capacity);

        for (int i = 0; i < size; i++)
        {
            new (&data[i]) T(other.data[i]);
        }
    }

    int capacity_value(void)
    {
        return capacity;
    }
    int length(void)
    {
        return size;
    }
    void resize()
    {
        capacity *= 2;
        T *new_data = (T *)malloc(sizeof(T) * capacity);

        for (int i = 0; i < size; i++)
        {
            new (&new_data[i]) T(data[i]);
            data[i].~T();
        }
        free(data);

        data = new_data;
    }

    void shrink()
    {
        capacity /= 2;
        T *new_data = (T *)malloc(sizeof(T) * capacity);

        for (int i = 0; i < size; i++)
        {
            new (&new_data[i]) T(data[i]);
            data[i].~T();
        }

        free(data);
        data = new_data;
    }

    void add(T num)
    {
        if (size >= capacity)
        {
            resize();
        }

        new (&data[size]) T(num);
        size++;
    }

    T &get(int index)
    {
        if (index < 0 || index >= size)
        {
            string error = "An error occured as it is an invalid index.";
            throw error;
        }

        return data[index];
    }

    const T &get(int index) const
    {
        if (index < 0 || index >= size)
        {
            string error = "An error occured as it is an invalid index.";
            throw error;
        }

        return data[index];
    }

    void remove(int index)
    {
        if (index < 0 || index >= size)
        {
            string error = "An error occured as it is an invalid index.";
            throw error;
        }

        data[index].~T();

        // If removed element is not the last element in the array
        if (index != size - 1) {
            new (&data[index]) T(data[size-1]);
            // Destroy the original
            data[size-1].~T();
        }
        size--;

        if (size > 0 && size < capacity / 2) {
            shrink();
        }
    }

    void fill(int count, T value)
    {
        if (count < 0 || count + size > capacity)
        {
            string error = "An error has occured.";
            throw error;
        }

        for (int i = 0; i < count; i++)
        {
            add(value);
        }
    }

    void clear()
    {
        for (int i = 0; i < size; i++) {
            data[i].~T();
        }

        size = 0;
    }

    T &operator[](int index)
    {
        return get(index);
    }

    const T &operator[](int index) const
    {
        return get(index);
    }
};

#endif
// --- END OF dynamic_array.h ---


// ==========================================
// --- START OF entity.h ---
// ==========================================

#ifndef ENTITY_H
#define ENTITY_H
#include "splashkit.h"
// Removed local include: #include "dynamic_array.h"


class Entity {
    protected:
        point_2d coordinates{};
        double health{};
        double movement_speed{};
        double max_health{};

    public:
        Entity(double x, double y, double health, double speed);
        void update();
        void take_damage(double amount);
        point_2d get_coordinates() const;
        double get_health() const;
        
};





#endif
// --- END OF entity.h ---


// ==========================================
// --- START OF projectile.h ---
// ==========================================

#ifndef PROJECTILE_H
#define PROJECTILE_H



class Projectile : public Entity {
    protected:
        vector_2d velocity{};
        bool is_deflected = false;
        bool is_friendly;
        double anim_timer{};
        int current_frame{};

    public:
        Projectile(point_2d start, point_2d target);

        void update(double dt);
        void draw();
        void deflect();
        bool get_is_deflected() const;
        bool get_is_friendly() const;
        void set_is_friendly(bool friendly);
};


#endif
// --- END OF projectile.h ---




#ifndef PLAYER_H
#define PLAYER_H
// Removed local include: #include "entity.h"

enum WeaponType {
    SWORD,
    BOW
};

enum FloorType {
    FLOOR_GRASS,
    FLOOR_STONE
};

enum BoonID {
    BOON_ARES_STRIKE,
    BOON_ARES_SWIFTNESS,

    BOON_ZEUS_STRIKE,
    BOON_ZEUS_SWIFTNESS,
    BOON_ZEUS_DASH
};

struct Boon {
    BoonID id;
    string name;
    string description;
    color theme;
};

class Player : public Entity {
    protected: 
        double dash_cooldown{};
        bool is_dashing{};
        double dash_active_timer{};
        bool is_swinging{};
        vector_2d swing_dir{};
        double swing_timer{};
        double sword_cooldown{};
        double i_frame_timer{};
        point_2d hitbox{};
        dynamic_array<int> hit_list{};
        vector_2d knockback_vec{};
        double knockback_timer{};
        double sword_damage{25.0};
        vector_2d current_velocity{};
        bool is_parrying{};
        double parry_timer{};
        double parry_cooldown{};
        animation player_animation{};
        animation_script player_anim_script{};
        bool is_facing_left{};
        string current_animation{};
        WeaponType weapon = SWORD;
        bool arrow_ready{};
        point_2d arrow_target{};
        dynamic_array<Boon> active_boons{};
        double step_timer;
        FloorType current_floor;
        bool parry_landed = false;
        int melee_combo_step{};
        double combo_window_timer{};
        double combo_damage_mult{1.0};
        bool just_dashed{};

    public:
        Player(double x, double y);
        void handle_input(double dt);
        void dash();
        void draw();
        void attack(point_2d target);
        void draw_hitbox();
        point_2d get_attack_hitbox() const;
        bool get_is_swinging() const;
        bool has_hit(int enemy_id);
        void add_to_hitlist(int enemy_id);
        bool is_dead() const;
        bool is_invincible() const;
        void take_damage(double amount);
        void apply_knockback(vector_2d force, double time);
        void set_coordinates(double new_x, double new_y);
        void increase_damage(double amount);
        void increase_speed(double amount);
        void heal(double amount);
        double get_sword_damage();
        void reset_stats();
        vector_2d get_curr_velocity() const;
        void attempt_parry();
        bool get_is_parrying() const;
        bool is_hitbox_active() const;
        void equip_weapon(WeaponType w);
        WeaponType get_weapon() const;
        bool pull_trigger();
        point_2d get_arrow_target() const;
        void draw_hud();
        void add_boon(Boon b);
        bool has_boon(BoonID target_id);
        void set_floor_type(FloorType f);
        bool pull_trigger(point_2d &target_out);
        bool get_parry_landed() const {return parry_landed;}
        void set_parry_landed(bool val) {parry_landed = val;}
        void grant_iframes(double duration);
        void reset_parry_cooldown();
        void extend_parry();
        double get_combo_mult() const;
        bool get_and_clear_dash_event();
};


#endif
// --- END OF player.h ---



// --- START OF enemy.h ---


#ifndef ENEMY_H
#define ENEMY_H

// Removed local include: #include "entity.h"
// Removed local include: #include "player.h"
// Removed local include: #include "projectile.h"

enum BossState {
    B_INTRO,
    B_WALKING,
    B_TRIPLE_BURST,
    B_SHOTGUN,
    B_LUNGE,
    B_MELEE_COMBO,
    B_GROUND_SLAM,
    B_PHASE_TRANSITION,
    B_SHADOW_STEP
};

struct Particle {
    point_2d position{};
    vector_2d velocity{};
    double lifetime{};
    double max_lifetime{};
    color base_color;
    double start_size = 8.0;
};

class Enemy : public Entity {
    protected:
        int id{};
        double attack_range{};
        double damage_output{};
        double reload_timer{};
        vector_2d velocity{};
        double stun_timer{};
        double flash_timer{};
        double windup_timer{};
        bool is_winding_up{};
        bool is_melee{};
        double ultimate_cooldown{};

    public:
        Enemy(double x, double y, int assigned_id, double health, double speed);
        virtual ~Enemy();
        virtual void draw(bool show_debug);
        virtual void update(const Player& target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles);
        int get_id() const;
        virtual void apply_knockback(vector_2d force, double duration);
        virtual bool has_exploded() const { return false; }
        bool is_stunned() const {return stun_timer > 0;}
        virtual bool check_and_clear_aoe(double &out_radius) {return false;}
        virtual double get_collision_radius() const;
        virtual double get_damage_radius() const;
        virtual bool get_is_enraged() const  {return false;}
    
};

class ShooterEnemy : public Enemy {
    private:
        bool is_facing_left = false;
    public:
        ShooterEnemy(double x, double y, int assigned_id);
        void draw(bool show_debug);
        void update(const Player &target, double dt, dynamic_array <Projectile> &projectiles, dynamic_array<Particle> &particles);
        animation enemy_anim;
        string current_anim_name = "Idle";
};

class ChaserEnemy: public Enemy {
    private:
        animation chaser_anim;
        string current_anim_name;
        bool is_facing_left;
    public:
        ChaserEnemy(double x, double y, int assigned_id);
        void draw(bool show_debug);
        void update(const Player &target, double dt, dynamic_array <Projectile> &projectiles, dynamic_array<Particle> &particles);
};

class BossEnemy: public Enemy {
    private:
        BossState current_state{};
        double state_timer{};
        double action_timer{};
        int burst_counter{};
        point_2d locked_target{};
        bool is_enraged = false;
        bool trigger_aoe = false;
        animation boss_anim;
        string current_anim_name;
        bool is_facing_left = false;
        double display_health{};

    public:
        BossEnemy(double x, double y, int assigned_id);
        void update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles);
        void draw(bool show_debug);
        void apply_knockback(vector_2d force, double duration);
        bool check_and_clear_aoe(double &out_radius);
        double get_collision_radius() const;
        double get_damage_radius() const;
        bool get_is_enraged() const {return is_enraged;}
};

class KamikazeEnemy: public Enemy {
    protected:
        double fuse_timer = 4.0;
        bool is_exploded = false;
        bool is_triggered{};

    public:
        KamikazeEnemy(double x, double y, int assigned_id);
        void draw(bool show_debug);
        void update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles);
        bool has_exploded() const;
};

#endif
// --- END OF enemy.h ---



// --- START OF room.h ---


#ifndef ROOM_H
#define ROOM_H
// Removed local include: #include "dynamic_array.h"
#include "splashkit.h"


enum reward_type {
    HEAL, 
    ARES,
    ZEUS,
    BOSS
};

class RoomNode {
    private:
        int enemy_count{};
        reward_type room_reward{};
        dynamic_array<RoomNode*> next_doors{}; // ADG

    public:
        RoomNode(int enemies, reward_type reward);
        void add_door(RoomNode* next_room);
        dynamic_array<RoomNode*> get_doors();
        reward_type get_reward();
        int get_enemies();
};



#endif
// --- END OF room.h ---



// --- START OF level_manager.h ---


#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

// Removed local include: #include "room.h"
// Removed local include: #include "enemy.h"
// Removed local include: #include "dynamic_array.h"
#include "splashkit.h"

// Moved from GameController!
struct PhysicalDoor {
    rectangle bounds;
    RoomNode* destination;
};

class LevelManager {
    private:
        RoomNode* root_node;
        RoomNode* current_room;
        dynamic_array<RoomNode*> all_nodes;
        dynamic_array<PhysicalDoor> active_doors;

        

    public:
        LevelManager();
        ~LevelManager();

        void generate_graph();
        
        // Pass the enemy array by reference so the Manager can fill it!
        void spawn_wave(dynamic_array<Enemy*> &enemy_arr, int screen_w, int screen_h);
        
        // Door and Room Getters
        RoomNode* get_current_room();
        void set_current_room(RoomNode* next_room);
        
        void generate_doors(int screen_w, int screen_h);
        dynamic_array<PhysicalDoor>& get_active_doors();
        reward_type get_random_reward();
        void clear_doors();
};

#endif
// --- END OF level_manager.h ---


// ==========================================
// --- START OF GameController.h ---
// ==========================================

#ifndef CONTROLLER_H
#define CONTROLLER_H

// Removed local include: #include "player.h"
// Removed local include: #include "dynamic_array.h"
// Removed local include: #include "enemy.h"
// Removed local include: #include "projectile.h"
// Removed local include: #include "level_manager.h"
#include <fstream>

enum MusicTrack {
    TRACK_NONE,
    TRACK_HUB,
    TRACK_BATTLE,
    TRACK_BOSS,
    TRACK_BOSS_2,
    TRACK_DEFEAT,
    TRACK_VICTORY
};

enum game_state {
    PLAYING,
    DEAD,
    VICTORY,
    DRAFTING,
    HUB,
    MAIN_MENU
};

struct RewardDrop {
    bool is_active{};
    point_2d position;
    reward_type type;
};

struct DangerZone {
    point_2d position;
    double radius;
    double timer;
};




class GameController {
    private:
        Player player{640, 360};
        dynamic_array<Enemy*> enemy_arr{};
        dynamic_array<Projectile> projectiles{};
        dynamic_array<Particle> particles;
        dynamic_array<DangerZone> danger_zones{};
        MusicTrack current_track;
        
        game_state state{};
        double hit_pause_timer{};
        double screen_shake_timer{};
        // --- NEW ARCHITECTURE ---
        LevelManager level_manager; 
        bool show_debug = false;
        RewardDrop current_reward{};
        bool room_cleared = false;
        dynamic_array<Boon> current_draft_options;
        bool is_paused;
        double hub_float_angle{};
        double arena_radius = 1200;
        point_2d arena_center{};
        animation fire_wall_anim{};
        double death_timer{};
        int current_score = 0;
        int combo_multiplier = 1;
        double combo_timer = 0;
        int top_scores[5] = {0};
        bool is_viewing_leaderboard = false;
        double run_timer{};
        bool time_bonus_applied = false;
        double victory_timer{};
        bool score_sound_played{};

    public:
        GameController();
        ~GameController();
        
        void run(double dt);
        void update_hub(double dt);
        void update_playing(double dt);
        
        void draw_death_screen(double dt);
        void draw_victory_screen(double dt);
        void reset();
        
        string reward_to_string(reward_type r);
        void apply_reward(reward_type reward);
        void update_drafting();
        void populate_draft_menu(reward_type god_type);
        void update_main_menu();
        void draw_pause_menu();
        void load_leaderboard();
        void save_new_score(int final_score);
};

#endif
// --- END OF GameController.h ---



// --- START OF entity.cpp ---


// Removed local include: #include "entity.h"
// Removed local include: #include "dynamic_array.h"


Entity::Entity(double x, double y, double health, double speed)
{
    coordinates.x = x;
    coordinates.y = y;
    this->health = health;
    this->max_health = health;
    movement_speed = speed;
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

double Entity::get_health() const
{
    return health;
}





// --- END OF entity.cpp ---


// ==========================================
// --- START OF projectile.cpp ---
// ==========================================

// Removed local include: #include "projectile.h"

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




// --- END OF projectile.cpp ---


// ==========================================
// --- START OF player.cpp ---
// ==========================================

// Removed local include: #include "player.h"
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


// --- END OF player.cpp ---


// ==========================================
// --- START OF enemy.cpp ---
// ==========================================

// Removed local include: #include "enemy.h"
#include <cmath>

Enemy::Enemy(double x, double y, int assigned_id, double health, double speed)
    : Entity(x, y, health, speed)
{
    id = assigned_id;
    reload_timer = 1;
}

Enemy::~Enemy() {}

void Enemy::draw(bool show_debug)
{

    double hp_percent = health / max_health;

    if (hp_percent < 0)
    {
        hp_percent = 0;
    }

    fill_rectangle(COLOR_BLACK, coordinates.x - 20, coordinates.y - 20, 40, 5);
    fill_rectangle(COLOR_GREEN, coordinates.x - 20, coordinates.y - 20, 40 * hp_percent, 5);
}

void Enemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    (void)target;
    (void)projectiles;

    // Part 1: Physics
    coordinates.x += velocity.x * dt;
    coordinates.y += velocity.y * dt;

    velocity.x -= velocity.x * 8.0 * dt;
    velocity.y -= velocity.y * 8.0 * dt;

    if (flash_timer > 0)
    {
        flash_timer -= dt;
    }

    if (stun_timer > 0)
    {
        stun_timer -= dt;
    }

    int border = 60;
    if (coordinates.x < border) coordinates.x = border;
    if (coordinates.x > screen_width() - border) coordinates.x = screen_width() - border;
    if (coordinates.y < border) coordinates.y = border;
    if (coordinates.y > screen_height() - border) coordinates.y = screen_height() - border;
}

ShooterEnemy::ShooterEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 80, 250)
{
    enemy_anim = create_animation(animation_script_named("HellhoundAnim"), "Idle");
}

void ShooterEnemy::draw(bool show_debug)
{
    drawing_options opt = option_with_animation(enemy_anim);

    if (is_facing_left) {
        opt.flip_y = true;
    }

    if (flash_timer > 0)
    {
        fill_circle(COLOR_WHITE, coordinates, 20);
    }
    else
    {
        draw_bitmap("Hellhound", coordinates.x - 40, coordinates.y - 30, opt);
    }

    if (is_winding_up)
    {
        double flash_rate = 0.1 - (0.07 * (1 - windup_timer / 0.3));
        if (std::fmod(windup_timer, flash_rate * 2) < flash_rate)
        {
            draw_text("!", COLOR_ORANGE, "TitleFont", 24, coordinates.x - 5, coordinates.y - 45);
        }
    }

    Enemy::draw(show_debug);
}

void ShooterEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles);
    
    if (target.get_coordinates().x < coordinates.x) {
        is_facing_left = false;
    } else {
        is_facing_left = true;
    }
    if (stun_timer > 0)
    {
        return;
    }

    // AI: Walk to player
    double dist = point_point_distance(coordinates, target.get_coordinates());
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());

  
    if (vector_magnitude(raw_dir) > 0)
    {
        vector_2d move_dir = unit_vector(raw_dir);

        if (!is_winding_up) {
            if (dist > 150) {
                coordinates.x += move_dir.x * movement_speed * dt;
                coordinates.y += move_dir.y * movement_speed * dt;
            } else {
                int strafe_dir = (get_id() % 2 == 0) ? 1 : -1; 
                
                // The magical Perpendicular Vector math (-Y, X)
                double side_x = -move_dir.y * strafe_dir;
                double side_y = move_dir.x * strafe_dir;

                // Move sideways at 70% speed so the player can still easily catch them
                double strafe_speed = movement_speed * 0.6; 
                
                coordinates.x += side_x * strafe_speed * dt;
                coordinates.y += side_y * strafe_speed * dt;
            }
        }
        
    }

    // --- ANIMATION SWITCH (same pattern as guide's assign_animation calls) ---
    string desired_anim = (stun_timer > 0 || is_winding_up) ? "Idle" : "Run";

    if (desired_anim != current_anim_name)
    {
        current_anim_name = desired_anim;
        assign_animation(enemy_anim, desired_anim, animation_script_named("HellhoundAnim"));
    }

    // Always update the animation every frame
    update_animation(enemy_anim);

    // AI: Shooting Logic
    if (reload_timer <= 0)
    {
        if (!is_winding_up) {
            is_winding_up = true;
            windup_timer = 0.3;
        }
        else{
            windup_timer -= dt;
            if (windup_timer <= 0)
            {
                projectiles.add(Projectile(coordinates, target.get_coordinates()));
                reload_timer = 1;
                is_winding_up = false;
            }
        }
    } else {
        reload_timer -= dt;
    }
}

ChaserEnemy::ChaserEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 50, 300) {
        chaser_anim = create_animation(animation_script_named("ChaserAnim"), "Run");
        current_anim_name = "Run";
        is_facing_left = false;
    }

void ChaserEnemy::draw(bool show_debug)
{
    drawing_options opt = option_with_animation(chaser_anim);
    if (is_facing_left) {
        opt.flip_y = true;
    }

    if (flash_timer > 0)
        fill_circle(COLOR_WHITE, coordinates, 20);
    else {
        double draw_x = coordinates.x - 48;
        double draw_y = coordinates.y - 56;

        draw_bitmap("ChaserSprite", draw_x, draw_y , opt);
    }

    Enemy::draw(show_debug); // Draw the health bar!
}

void ChaserEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles); // Run Base Physics

    // Face player
    if (target.get_coordinates().x < coordinates.x) {
        is_facing_left = true;
    } else {
        is_facing_left = false;
    }
    if (stun_timer > 0)
        return; // Abort AI if stunned!

    // AI: Sprint at player
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0)
    {
        vector_2d move_dir = unit_vector(raw_dir);

        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }

    if (dt > 0) {
        update_animation(chaser_anim);
    }
}

BossEnemy::BossEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 4000, 150)
{
    current_state = B_INTRO;
    state_timer = 2.5;
    action_timer = 0;
    burst_counter = 0;
    boss_anim = create_animation(animation_script_named("BossAnim"), "Idle");
    current_anim_name = "Idle";
    
}

void BossEnemy::draw(bool show_debug)
{ 
    // Draw the Telegraphs based onhe State!
    if (current_state == B_GROUND_SLAM && action_timer > 0) {
        double max_time = is_enraged ? 1.0 : 1.5;
        double progress = 1.0 - (action_timer / max_time);
        
        fill_circle(rgba_color(255, 0, 0, 30), coordinates.x, coordinates.y, 200);
        fill_circle(rgba_color(255, 0, 0, 120), coordinates.x, coordinates.y, 200 * progress);

    } else if (current_state == B_PHASE_TRANSITION) {
        double progress = 1.0 - (state_timer / 2.5);
        if (progress > 1.0) progress = 1.0;
        
        // Dim the lights!
        fill_rectangle(rgba_color(0.0, 0.0, 0.0, 100 * progress), camera_x(), camera_y(), screen_width(), screen_height());
        
        // Draw the massive ticking time-bomb circle
        fill_circle(rgba_color(255, 0, 0, 30), coordinates.x, coordinates.y, 200);
        fill_circle(rgba_color(255, 0, 0, 150), coordinates.x, coordinates.y, 200 * progress);
        
        // Draw a blinding inner aura that throbs
        fill_circle(rgba_color(255, 50, 50, 100), coordinates.x, coordinates.y, 80 + (rnd() * 30));
    } 
    else if (current_state == B_SHADOW_STEP) {
        
        // 1. THE BLINKING LIGHT CUES (Only happens on Beats 1 and 2)
        // Check if we are in the first 0.15s of the beat
        fill_rectangle(rgba_color(0, 0, 0, 210), camera_x(), camera_y(), screen_width(), screen_height());

    double cx = camera_x() + (screen_width() / 2.0);
    double cy = camera_y() + (screen_height() / 2.0);

    // 2. THE TELEGRAPHS (Burst 1, 3, 5)
    if (burst_counter == 1) { 
        // Slash 1 Cue
        if (std::fmod(action_timer, 0.1) > 0.05) { // Flickering warning
            for (int offset = -400; offset <= 400; offset += 400) {
                for(int i = -2; i <=2 ;i++) {
                    draw_line(COLOR_YELLOW, cx - 800, cy - 500 + offset + i, cx + 800, cy + 500 + offset+ i);
                }
            }
        }
        draw_text("PARRY!", COLOR_YELLOW, "TitleFont", 40, cx + 100, cy - 100);
    }
    else if (burst_counter == 3) {
        // Slash 2 Cue: Top-Right to Bottom-Left
        if (std::fmod(action_timer, 0.1) > 0.05) {
            for (int offset = -400; offset <= 400; offset += 400) {
                for (int i = -2; i <= 2; i++) {
                    draw_line(COLOR_YELLOW, cx + 800, cy - 500 + offset + i, cx - 800, cy + 500 + offset + i);
                }
            }
        }
        draw_text("PARRY!", COLOR_YELLOW, "TitleFont", 40, cx - 200, cy + 50);
    }
    else if (burst_counter == 5) {
        // Final Cut Cue: Massive Horizontal Beam
        if (std::fmod(action_timer, 0.1) > 0.05) {
            
            fill_rectangle(rgba_color(255, 0, 0, 150), camera_x(), cy - 40, screen_width(), 80);
            draw_rectangle(COLOR_RED, camera_x(), cy - 40, screen_width(), 80);
            
            // Generate a random offset between -3 and +3 pixels
            double jitter_x = rnd(6) - 3;
            double jitter_y = rnd(6) - 3;
            
            // Text shakes violently to sell the charging power!
            draw_text("FINAL STRIKE", COLOR_RED, "TitleFont", 50, cx - 160 + jitter_x, cy - 90 + jitter_y);
        }
    }

    // 3. THE ACTIVE HITS (Burst 2, 4, 6)
    if (burst_counter == 2) {
        // FULL SCREEN FLASH (Tints the entire room blood red to show the AoE is global)
        fill_rectangle(rgba_color(150, 0, 0, 100), camera_x(), camera_y(), screen_width(), screen_height());

        // Thick Red/White Slashes x3
        for (int offset = -400; offset <= 400; offset += 400) {
            for (int i = -15; i <= 15; i++) {
                draw_line(COLOR_RED, cx - 800, cy - 500 + offset + i, cx + 800, cy + 500 + offset + i);
            }
            for (int i = -4; i <= 4; i++) {
                draw_line(COLOR_WHITE, cx - 800, cy - 500 + offset + i, cx + 800, cy + 500 + offset + i);
            }
        }
    }
    else if (burst_counter == 4) {
        // FULL SCREEN FLASH
        fill_rectangle(rgba_color(150, 0, 0, 100), camera_x(), camera_y(), screen_width(), screen_height());

        // Thick Red/White Slashes x3 (Flipped)
        for (int offset = -400; offset <= 400; offset += 400) {
            for (int i = -15; i <= 15; i++) {
                draw_line(COLOR_RED, cx + 800, cy - 500 + offset + i, cx - 800, cy + 500 + offset + i);
            }
            for (int i = -4; i <= 4; i++) {
                draw_line(COLOR_WHITE, cx + 800, cy - 500 + offset + i, cx - 800, cy + 500 + offset + i);
            }
        }
    }
    else if (burst_counter == 6) {
        // Blinding Screen Nuke
       if (std::fmod(action_timer, 0.08) > 0.04) {
            // Frame A: Blinding White screen with a massive Red tear
            fill_rectangle(COLOR_WHITE, camera_x(), camera_y(), screen_width(), screen_height());
            fill_rectangle(COLOR_DARK_RED, camera_x(), cy - 100, screen_width(), 200);
        } else {
            // Frame B: Pitch Black screen with a thin White tear
            fill_rectangle(rgba_color(10, 10, 15, 255), camera_x(), camera_y(), screen_width(), screen_height());
            fill_rectangle(COLOR_WHITE, camera_x(), cy - 20, screen_width(), 40);
            }
        }
    }

    // 2. Draw the actual Boss Sprite!
    drawing_options opts = option_with_animation(boss_anim);
    if (is_facing_left) {
        opts.flip_y = true;
    }
    opts.scale_x = 2.5;
    opts.scale_y = 2.5;

    double draw_x = coordinates.x - 200;
    double draw_y = coordinates.y - 150;

    // --- TWEAK THESE OFFSETS ---
    // If the hitbox is too far to the right, it means the sprite is drawing too far to the left.
    // Add positive X to push the sprite to the right!
    double offset_x = 130; // Tweak this number! 
    double offset_y = 10; // Tweak this number!

    draw_x += offset_x;
    draw_y += offset_y;

    if (flash_timer > 0) {
        draw_x += (rnd() * 16- 8);
        draw_y += (rnd() * 16 - 8);
    }
    
    // Draw the boss perfectly centered on its collision coordinates
    if (current_state != B_SHADOW_STEP) {
        draw_bitmap("Boss", draw_x, draw_y, opts);
    }

    // ANIME GLINT
    if (current_state == B_MELEE_COMBO && action_timer > 0) {
        if (std::fmod(action_timer, 0.1) > 0.05) {
            double glint_x = is_facing_left ? coordinates.x - 40 : coordinates.x + 40;
            fill_circle(COLOR_WHITE, glint_x, coordinates.y - 20, 15);
            fill_circle(COLOR_YELLOW, glint_x, coordinates.y - 20, 10);

            fill_rectangle(COLOR_WHITE, glint_x - 30, coordinates.y - 22, 60, 4);
            fill_rectangle(COLOR_WHITE, glint_x - 2, coordinates.y - 50, 4, 60);
        }
    }

    // ==========================================
    // --- FLOATING COMBAT TEXT ---
    // ==========================================
    string attack_text = "";
    color text_color = COLOR_WHITE;

    if (current_state == B_TRIPLE_BURST) {
        attack_text = "TRIPLE BURST!";
        text_color = COLOR_RED;
    } 
    else if (current_state == B_SHOTGUN) {
        attack_text = "SHOTGUN!";
        text_color = COLOR_ORANGE;
    } 
    else if (current_state == B_LUNGE) {
        attack_text = "LUNGE!";
        text_color = COLOR_YELLOW;
    } 
    else if (current_state == B_MELEE_COMBO) {
        attack_text = "COMBO!";
        text_color = COLOR_DARK_RED;
    }

    // Only draw the text if the boss is actually attacking
    if (attack_text != "") {
        draw_text(attack_text, text_color, "TitleFont", 20, coordinates.x - 45, coordinates.y - 85);
    }

    // ==========================================
    // --- THE GRAND BOSS HEALTH BAR ---
    // ==========================================
    
    double active_hp = (current_state == B_INTRO) ? display_health : health;
    double hp_percent = active_hp / max_health;
    if (hp_percent < 0) hp_percent = 0;

    double screen_center = camera_x() + (screen_width() / 2.0);
    double bar_y = camera_y() + 60; // Pushed down slightly to give the text room
    
    double bar_width = 800;
    double start_x = screen_center - (bar_width / 2.0);

    if (current_state == B_INTRO) {
        // As the timer counts down, the screen gets darker
        double intensity = state_timer / 2.5; 
        
        // 1. Dim the whole screen
        fill_rectangle(rgba_color(0.0, 0.0, 0.0, 180 * intensity), camera_x(), camera_y(), screen_width(), screen_height());
        
        // 2. Pulse a violent, jittering crimson aura around the crouched boss
        double aura_radius = 80 + (rnd() * 20); 
        fill_circle(rgba_color(150.0, 0.0, 0.0, 100 * intensity), coordinates.x, coordinates.y, aura_radius);
    }

    // --- DRAW THE BAR ---
    fill_rectangle(rgba_color(15, 5, 5, 255), start_x, bar_y, bar_width, 20);
    draw_rectangle(COLOR_DARK_GRAY, start_x - 2, bar_y - 2, bar_width + 4, 24);
    fill_rectangle(COLOR_RED, start_x, bar_y, bar_width * hp_percent, 20);

    // --- PERFECTLY CENTERED TITLE ---
    string boss_title = "V A E L E N ,   T H E   A B Y S S   W A T C H E R";
    int font_size = 26;
    font title_font = font_named("TitleFont");
    
    // Let SplashKit calculate the exact pixel width of the string
    double text_w = text_width(boss_title, title_font, font_size);
    
    // Shift the text left by exactly half its width
    draw_text(boss_title, COLOR_WHITE, title_font, font_size, screen_center - (text_w / 2.0), bar_y - 35);

    if (show_debug) {
        // A bright green ring showing the actual 120px collision zone
        draw_circle(COLOR_LIME_GREEN, coordinates.x, coordinates.y, get_collision_radius());
        
        // A red dot showing the exact mathematical center of the boss
        fill_circle(COLOR_RED, coordinates.x, coordinates.y, 5);
    }
}



void BossEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles);
    if (target.get_coordinates().x < coordinates.x) {
        is_facing_left = true;
    } else is_facing_left = false;


    if (ultimate_cooldown > 0) {
        ultimate_cooldown -= dt;
    }
    if (!is_enraged && health <= max_health / 2) {
        is_enraged = true;
        current_state = B_PHASE_TRANSITION;
        play_sound_effect("BossRoar", 1, 0.7f);
        state_timer = 3.0;
        action_timer = 0;
        movement_speed = 250;
        ultimate_cooldown = 0;
    }

    double downtime = is_enraged ? 1.0 : 2.0;

    // ==========================================
    // --- THE FINITE STATE MACHINE (FSM) ---
    // ==========================================
    switch (current_state)
    {
    
    case B_INTRO:
    {
        if (display_health < max_health) {
            // Fill rate = Total Health / Total Time
            display_health += (max_health / 2.5) * dt; 
        }

        // 3. Countdown to the fight
        state_timer -= dt;
        if (state_timer <= 0) {
            display_health = max_health; // Snap to full just in case
            current_state = B_WALKING;   // Let the battle begin!
            state_timer = downtime;
        }
        break;
    }
    case B_WALKING:
    {

        if (is_enraged && ultimate_cooldown <= 0 && point_point_distance(coordinates, target.get_coordinates()) < 300) {
            current_state = B_SHADOW_STEP;
            action_timer = 0.5;
            burst_counter = 0;
            ultimate_cooldown = 12.0;

            if (current_anim_name != "Idle") {
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            }
            
            break;
        }

        if (stun_timer > 0) {
            if (current_anim_name != "Hurt") {
                assign_animation(boss_anim, "Hurt");
                current_anim_name = "Hurt";
            }
        }
        else if (current_anim_name != "Run") {
            assign_animation(boss_anim, "Run");
            current_anim_name = "Run";
        }
        // Simply walk toward the player
        vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
        if (vector_magnitude(raw_dir) > 0)
        {
            vector_2d move_dir = unit_vector(raw_dir);
            coordinates.x += move_dir.x * movement_speed * dt;
            coordinates.y += move_dir.y * movement_speed * dt;
        }

        // Count down to the next attack
        state_timer -= dt;
        if (state_timer <= 0)
        {
            // Roll a 3-sided dice to pick a random attack
            float roll = rnd();
            if (roll < 0.20)
            {
                current_state = B_TRIPLE_BURST;
                action_timer = is_enraged ? 0.3 : 0.5;
                burst_counter = 0;
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            }
            else if (roll < 0.40)
            {
                current_state = B_SHOTGUN;
                action_timer = is_enraged ? 0.4: 0.6; // 0.6s Telegraph
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            }
            else if (roll < 0.60)
            {
                current_state = B_LUNGE;
                action_timer = is_enraged ? 0.5 : 0.8; // 0.8s Telegraph
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";

            } else if (roll < 0.80){
                current_state = B_MELEE_COMBO;
                action_timer = is_enraged ? 0.3 : 0.5;
                burst_counter = 0;
                assign_animation(boss_anim, "Idle");
                current_anim_name = "Idle";
            } else {
                current_state = B_GROUND_SLAM;
                action_timer = is_enraged ? 0.5 : 1.5;
                assign_animation(boss_anim, "JumpWindup");
                current_anim_name = "JumpWindup";
            }
        }
        break;
    }

    case B_TRIPLE_BURST:
    {
        action_timer -= dt;
        if (action_timer <= 0)
        {

            // Fire a ring of 12 projectiles
            for (int i = 0; i < 12; i++)
            {
                // Offset the angle slightly for each burst wave (burst_counter * 15 degrees)
                double angle = (i * 30) + (burst_counter * 15);

                point_2d proj_target;
                proj_target.x = coordinates.x + cosine(angle) * 100;
                proj_target.y = coordinates.y + sine(angle) * 100;
                projectiles.add(Projectile(coordinates, proj_target));
            }

            burst_counter++;
            int max_waves = is_enraged ? 5 : 3;
            if (burst_counter >= max_waves)
            {
                // Finished firing 3 waves, go back to walking!
                current_state = B_WALKING;
                state_timer = downtime;
            }
            else
            {
                // Wait 0.3 seconds before firing the next ring
                action_timer = is_enraged ? 0.15 : 0.3;
            }
        }
        break;
    }

    case B_SHOTGUN:
    {
        action_timer -= dt;
        if (action_timer <= 0)
        {
            // 1. Find the exact angle to the player
            vector_2d dir = vector_point_to_point(coordinates, target.get_coordinates());
            double base_angle = vector_angle(dir);
            int spread = is_enraged ? 6 : 3;

            // 2. Fire 7 bullets in a cone spread
            for (int i = -spread; i <= spread; i++)
            {
                double angle = base_angle + (i * 10); // Spread bullets 10 degrees apart

                point_2d proj_target;
                proj_target.x = coordinates.x + cosine(angle) * 100;
                proj_target.y = coordinates.y + sine(angle) * 100;
                projectiles.add(Projectile(coordinates, proj_target));
            }

            current_state = B_WALKING;
            state_timer = downtime;
        }
        break;
    }

    case B_LUNGE:
    {
        if (action_timer > 0)
        {
            // Phase 1: Telegraphing (Standing perfectly still)
            action_timer -= dt;

            if (action_timer >= 0.2)
            {
                point_2d p_pos = target.get_coordinates();
                vector_2d p_vel = target.get_curr_velocity();
                locked_target.x = p_pos.x + (p_vel.x * 0.15);
                locked_target.y = p_pos.y + (p_vel.y * 0.15);
            }

            if (action_timer <= 0)
            {
                assign_animation(boss_anim, "NormalAttack");
                current_anim_name = "NormalAttack";
                vector_2d raw_dir = vector_point_to_point(coordinates, locked_target);
                if (vector_magnitude(raw_dir) > 0)
                {
                    raw_dir = unit_vector(raw_dir);
                    int lunge_spd = is_enraged ? 2500 : 2000;

                    velocity.x = raw_dir.x * lunge_spd;
                    velocity.y = raw_dir.y * lunge_spd;
                    play_sound_effect("BossLunge");
                }
                state_timer = 0.7;
            }
        }
        else
        {
            state_timer -= dt;
            if (state_timer <= 0)
            {
                current_state = B_WALKING;
                state_timer = downtime;
            }
        }
        break;
    }

    case B_MELEE_COMBO:
    {
        // PHASE 1: The Initial Telegraph 
        // (Boss stands still in Idle before starting the flurry)
        if (action_timer > 0) {
            action_timer -= dt;
            if (action_timer <= 0) {
                // Telegraph is over! Force the first swing to trigger instantly.
                state_timer = 0; 
            }
        } 
        // PHASE 2: The Relentless Flurry
        else {
            state_timer -= dt; 
            
            // The exact millisecond a swing finishes (or on the very first swing):
            if (state_timer <= 0) {
                int max_combo = is_enraged ? 6 : 3;

                if (burst_counter >= max_combo) {
                    // Combo complete! Walk away.
                    current_state = B_WALKING;
                    state_timer = downtime;
                } 
                else {
                    // === INSTANT NEXT SWING (ZERO COOLDOWN) ===
                    if (burst_counter % 2 == 0) {
                        assign_animation(boss_anim, "NormalAttack");
                        current_anim_name = "NormalAttack";
                        play_sound_effect("BossSwing", 1, 0.7f);
                    } else {
                        assign_animation(boss_anim, "CrouchAttack");
                        current_anim_name = "CrouchAttack";
                        play_sound_effect("BossSwing", 1, 0.7f);
                    }
                    
                    // Aim at the player
                    point_2d p_pos = target.get_coordinates();
                    vector_2d p_vel = target.get_curr_velocity();
                    locked_target.x = p_pos.x + (p_vel.x * 0.15);
                    locked_target.y = p_pos.y + (p_vel.y * 0.15);
                    
                    // Dash forward
                    vector_2d raw_dir = vector_point_to_point(coordinates, locked_target);
                    if (vector_magnitude(raw_dir) > 0) {
                        raw_dir = unit_vector(raw_dir);
                        int dash_spd = is_enraged ? 2500: 2000;
                        velocity.x = raw_dir.x * dash_spd;
                        velocity.y = raw_dir.y * dash_spd;
                    }

                    // --- SWORD SWING SPARKS ---
                    for (int p = 0; p < 20; ++p) {
                        Particle spark;
                        spark.position = coordinates;
                        spark.base_color = COLOR_YELLOW;
                        double spread = (rnd() * 90) - 45; // Wider arc
                        double angle = vector_angle(raw_dir) + spread;
                        
                        // Slow drift
                        double speed = 50 + (rnd() * 150); 
                        spark.velocity.x = cosine(angle) * speed;
                        spark.velocity.y = sine(angle) * speed;
                        
                        // Lingers behind him
                        spark.max_lifetime = 0.6 + (rnd() * 0.4); 
                        spark.lifetime = spark.max_lifetime;
                        
                        // Pitch black shadows
                        spark.base_color = rgba_color(10, 10, 15, 255); 
                        particles.add(spark);
                    }
                    
                    burst_counter++; 
                    
                    
                    state_timer = 0.7; 
                }
            }
        }
        break;
    }
    case B_GROUND_SLAM:
    {
        if (action_timer > 0) {
            action_timer -= dt;

            if (action_timer <= 0) {
                trigger_aoe = true;
                play_sound_effect("BossSlam", 1, 0.6f);
                assign_animation(boss_anim, "JumpSlam");
                current_anim_name = "JumpSlam";
                state_timer = 0.6;

                // --- THE ABYSSAL SLAM ERUPTION ---
                for (int p = 0; p < 100; ++p) { // Increased count for a denser cloud
                    Particle dust;
                
                    dust.start_size = 30 + (rnd() * 20);
                    
                    double angle = (360.0/100) * p;
                    double blast_radius = 200.0;
                    dust.position.x = coordinates.x + cosine(angle) *blast_radius;
                    dust.position.y = coordinates.y + sine(angle) * blast_radius;

                    dust.position.y += 40;
                    // Slower speed 
                    double speed = 50 + (rnd() * 100); 
                    dust.velocity.x = cosine(angle) * speed;
                    dust.velocity.y = sine(angle) * speed;
                    
                    // Lingers for a massive 1.5 seconds
                    dust.max_lifetime = 1.0 + (rnd() * 0.5); 
                    dust.lifetime = dust.max_lifetime;
                    
                    // Deep, dark purple/black color
                    dust.base_color = rgba_color(25, 5, 35, 255); 
                    particles.add(dust);
                }
            }
        } else {
            state_timer -=dt;
            if (state_timer <= 0) {
                current_state = B_WALKING;
                state_timer = downtime;
            }
        }
        break;
    }

    case B_PHASE_TRANSITION:
    {
        if (health < max_health / 2) {
            health = max_health / 2;
        }
        
        if (current_anim_name != "Idle") {
            assign_animation(boss_anim, "Idle");
            current_anim_name = "Idle";
        }

        Particle aura;
        aura.position.x = coordinates.x + (rnd() * 80 - 40);
        aura.position.y = coordinates.y + (rnd() * 80 - 40);
        aura.velocity.y = -150 - (rnd() * 150); // Float upwards fast
        aura.velocity.x = (rnd() * 100) - 50;
        aura.max_lifetime = 0.4 + rnd() * 0.4;
        aura.lifetime = aura.max_lifetime;
        aura.base_color = rgba_color(200, 0, 0, 255); // Bright blood red
        aura.start_size = 15;
        particles.add(aura);

        state_timer -= dt;
        if (state_timer<= 0) {
            current_state = B_WALKING;
            state_timer = downtime;
        }
        break;
    }

    case B_SHADOW_STEP:
    {
        action_timer -= dt;
        if (action_timer <= 0) {
            burst_counter++;

            if (burst_counter == 1 || burst_counter == 3 || burst_counter == 5) {
                play_sound_effect("Clink", 1, 0.7f);
                
                // 0.3s after the first warning, but a terrifying 0.6s gap after the second!
                action_timer = 0.5;
            }
            // BEATS 3 & 4: THE STRIKES (Slash! Slash!)
            else if (burst_counter == 2 || burst_counter == 4) {
                play_sound_effect("BossSlam", 1, 0.8f);
                action_timer = 0.15; // Lightning fast 0.3s follow-up slash!
            }

            else if (burst_counter == 6) {
                play_sound_effect("Explosion", 1, 0.9f);
                action_timer = 0.2;
            }
            // THE END
            else { 
                current_state = B_WALKING;
                state_timer = downtime + 1.0; 
            }
    }
        break;
    }
}

    update_animation(boss_anim);
}


void BossEnemy::apply_knockback(vector_2d force, double duration)
{
    (void)force;
    flash_timer = 0.25;
    stun_timer = 0.25;

    if (current_state == B_MELEE_COMBO) {
        velocity.x = 0;
        velocity.y = 0;
        return;
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

KamikazeEnemy::KamikazeEnemy(double x, double y, int assigned_id)
    : Enemy(x, y, assigned_id, 40, 300) {}

void KamikazeEnemy::draw(bool show_debug)
{
    if (is_triggered)
    {
        double ratio = 1.0 - (fuse_timer / 1.5);
        draw_circle(rgba_color(255, 0, 0, 150), coordinates.x, coordinates.y, 100);

        double current_radius = 100 * ratio;
        fill_circle(rgba_color(255, 0, 0, 60), coordinates.x, coordinates.y, current_radius);

        double flash_rate = 0.05 + (fuse_timer * 0.1);
        draw_circle(rgba_color(255, 0, 0, 80), coordinates.x, coordinates.y, 100);
        if (std::fmod(fuse_timer, flash_rate * 2) > flash_rate)
        {
            fill_circle(COLOR_WHITE, coordinates, 15);
        }
        else
        {
            fill_circle(COLOR_RED, coordinates, 15);
        }
    }

    else
    {
        if (flash_timer > 0)
        {
            fill_circle(COLOR_WHITE, coordinates, 15);
        }

        else
        {
            fill_circle(COLOR_CYAN, coordinates, 15);
        }
    }

    Enemy::draw(show_debug);
}

void KamikazeEnemy::update(const Player &target, double dt, dynamic_array<Projectile> &projectiles, dynamic_array<Particle> &particles)
{
    Enemy::update(target, dt, projectiles, particles);
    // 1. THE SENSOR
    // If we haven't triggered yet, check the distance!
    if (!is_triggered)
    {
        double dist = point_point_distance(coordinates, target.get_coordinates());
        if (dist <= 150)
        {
            is_triggered = true; // ARM THE BOMB!
            movement_speed = 500;
            fuse_timer = 1; // Optional: Make it sprint faster when enraged!
        }
    }
    // 2. THE COUNTDOWN
    if (is_triggered)
    {
        fuse_timer -= dt;
        if (fuse_timer <= 0)
        {
            is_exploded = true; // Tell the engine to detonate!
            health = 0;         // Kill itself
            return;
        }
    }
    // 3. STUN CHECK
    if (stun_timer > 0)
        return;

    // 4. THE CHASE
    vector_2d raw_dir = vector_point_to_point(coordinates, target.get_coordinates());
    if (vector_magnitude(raw_dir) > 0)
    {
        vector_2d move_dir = unit_vector(raw_dir);
        coordinates.x += move_dir.x * movement_speed * dt;
        coordinates.y += move_dir.y * movement_speed * dt;
    }
}

bool KamikazeEnemy::has_exploded() const
{
    return is_exploded;
}

bool BossEnemy::check_and_clear_aoe(double &out_radius)
{
    if (trigger_aoe) {
        trigger_aoe = false;
        out_radius = 200.0;
        return true;
    }
    return false;
}

double Enemy::get_collision_radius() const
{
    return 30.0;
}

double BossEnemy::get_collision_radius() const
{
    return 70.0;
}

double Enemy::get_damage_radius() const
{
    return 18.0;
}
double BossEnemy::get_damage_radius() const
{
    if (current_state == B_SHADOW_STEP) {
        if ((burst_counter == 2 || burst_counter == 4 || burst_counter == 6) && action_timer <= 0.15) {
            // The hitbox only exists for a tiny 0.1s window when the timer has just reset
            return 1000.0;
        }
        return -50.0;
    }

    if (current_state == B_LUNGE || current_state == B_MELEE_COMBO) {
        if (action_timer <= 0) {
            return 50.0;
        }
    }

    return -50.0;

}
// --- END OF enemy.cpp ---


// ==========================================
// --- START OF room.cpp ---
// ==========================================

// Removed local include: #include "room.h"

RoomNode::RoomNode(int enemies, reward_type reward)
{
    enemy_count = enemies;
    room_reward = reward;
}

void RoomNode::add_door(RoomNode *next_room)
{
    next_doors.add(next_room);
}

dynamic_array<RoomNode*> RoomNode::get_doors()
{
    return next_doors;
}

reward_type RoomNode::get_reward()
{
    return room_reward;
}

int RoomNode::get_enemies()
{
    return enemy_count;
}


// --- END OF room.cpp ---


// ==========================================
// --- START OF level_manager.cpp ---
// ==========================================

// Removed local include: #include "level_manager.h"

LevelManager::LevelManager() {
    root_node = nullptr;
    current_room = nullptr;
}

LevelManager::~LevelManager() {
    for (int i = 0; i < all_nodes.length(); i++) {
        delete all_nodes[i];
    }
}

reward_type LevelManager::get_random_reward() {
    float roll = rnd();
    if (roll < 0.33) return HEAL;
    else if (roll < 0.66) return ARES;
    else return ZEUS;
}

void LevelManager::generate_graph() {
    // Clear old memory if we are restarting
    for (int i = 0; i < all_nodes.length(); i++) {
        delete all_nodes[i];
    }
    all_nodes.clear();
    active_doors.clear();

    int run_depth = 6;

    root_node = new RoomNode(0, HEAL);

    all_nodes.add(root_node);
    dynamic_array<RoomNode*> previous_layer;
    previous_layer.add(root_node);

    for (int depth = 1; depth < run_depth; ++depth) {
        dynamic_array<RoomNode *> current_layer;
        reward_type reward_1 = get_random_reward();
        reward_type reward_2 = get_random_reward();

        while (reward_1 == reward_2) {
            reward_2 = get_random_reward();
        }

        RoomNode* room_1 = new RoomNode(3 + depth, reward_1);
        RoomNode* room_2 = new RoomNode(3 + depth, reward_2);
        
        all_nodes.add(room_1);
        all_nodes.add(room_2);
        current_layer.add(room_1);
        current_layer.add(room_2);
        
        // Connect the graph
        for (int i = 0; i < previous_layer.length(); i++) {
            previous_layer[i]->add_door(room_1);
            previous_layer[i]->add_door(room_2);
        }

        previous_layer.clear();
        for (int i = 0; i < current_layer.length(); i++) {
            previous_layer.add(current_layer[i]);
        }
        
    }
    
    RoomNode *boss = new RoomNode(1, BOSS);
    all_nodes.add(boss);

    for (int i = 0; i < previous_layer.length(); i++) {
        previous_layer[i]->add_door(boss);
    }

    current_room = root_node;
}

void LevelManager::spawn_wave(dynamic_array<Enemy*> &enemy_arr, int screen_w, int screen_h) {
    if (current_room->get_reward() == BOSS) {
        enemy_arr.add(new BossEnemy(screen_w - 150, screen_h / 2, 0));
        return; 
    }

    int num_enemies = current_room->get_enemies();

    for (int i = 0; i < num_enemies; i++) {
        point_2d spawn{};
        if (rnd() < 0.5) {
            spawn.x = screen_w + 40;
            spawn.y = rnd(screen_h);
        } else {
            spawn.y = (rnd() < 0.5) ? -40 : screen_h + 40;
            spawn.x = screen_w / 2 + rnd(screen_w / 2); 
        }

        float roll = rnd();
        if (roll < 0.33) {
            enemy_arr.add(new ChaserEnemy(spawn.x, spawn.y, i));
        } else if (roll< 0.66) {
            enemy_arr.add(new ShooterEnemy(spawn.x, spawn.y, i));
        } else {
            enemy_arr.add(new KamikazeEnemy(spawn.x, spawn.y, i));
        }
    }
}

void LevelManager::generate_doors(int screen_w, int screen_h) {
    dynamic_array<RoomNode*> next_nodes = current_room->get_doors();
    
    int door_w = 60;
    int door_h = 100;
    int spacing = 200;
    int start_y = (screen_h - (next_nodes.length() * door_h + (next_nodes.length() - 1) * spacing)) / 2;

    for (int i = 0; i < next_nodes.length(); i++) {
        PhysicalDoor new_door;
        // Tweak door spawn here
        new_door.bounds = rectangle_from(screen_w - 180, start_y + (i * (door_h + spacing)), door_w, door_h);
        new_door.destination = next_nodes[i];
        active_doors.add(new_door);
    }
}

RoomNode* LevelManager::get_current_room() { 
    return current_room; 
}
void LevelManager::set_current_room(RoomNode* next_room) { 
    current_room = next_room; 
}
dynamic_array<PhysicalDoor>& LevelManager::get_active_doors() { 
    return active_doors; 
}
void LevelManager::clear_doors() { 
    active_doors.clear(); 
}
// --- END OF level_manager.cpp ---


// ==========================================
// --- START OF GameController.cpp ---
// ==========================================

// Removed local include: #include "GameController.h"


GameController::GameController() 
{
    state = MAIN_MENU; // Boot directly into the safe zone
    current_track = TRACK_NONE;
    load_font("TitleFont", "OptimusPrinceps.ttf");
    load_font("TextFont", "CaesarDressing-Regular.ttf");
    load_font("MenuFont", "SpectralSC-SemiBold.ttf");
    load_bitmap("Grass", "grass.png");
    load_bitmap("FloorBase", "stonetile.png");
    load_bitmap("FloorBase2", "stonetile2.png");
    load_sound_effect("Parry", "parry.wav");
    load_sound_effect("Sword", "sword.mp3");
    load_sound_effect("Impact", "impact.wav");
    load_sound_effect("Dash", "dash.mp3");
    load_sound_effect("StepStone", "concretestep.mp3");
    load_sound_effect("StepGrass", "grassstep.mp3");
    load_music("HubMusic", "trees.wav");
    load_music("BattleMusic", "battle.wav");
    load_music("BossMusic", "boss.wav");
    load_music("BossMusic2", "boss2.mp3");
    load_bitmap("SwordSprite", "Sword.png");
    load_bitmap("BowSprite", "Bow.png");
    load_bitmap("ArrowSprite", "Arrow.png");
    load_sound_effect("BossRoar", "bossroar.wav");
    load_sound_effect("BossSwing", "bossattack.mp3");
    load_sound_effect("BossLunge", "bossdash.mp3");
    load_sound_effect("BossSlam", "bossslam.mp3");
    load_sound_effect("Hurt", "hurt.mp3");
    load_music("DefeatMusic", "lose.wav");
    load_sound_effect("Explosion", "explosion.mp3");
    load_sound_effect("Combo", "combo.mp3");
    load_music("VictoryMusic", "win.wav");
    load_sound_effect("Clink", "clink.mp3");
    load_leaderboard();
    arena_center.x = screen_width() / 2.0;
    arena_center.y = screen_height() / 2.0;
    fire_wall_anim = create_animation(animation_script_named("FireAnim"), "Burn");
   

}

GameController::~GameController()
{
    // Only clean up the enemies
    // The LevelManager now handles deleting the graph nodes!
    for (int i = 0; i< enemy_arr.length(); i++) {
        delete enemy_arr[i];
    }
}

void GameController::run(double dt)
{
    if (key_typed(ESCAPE_KEY)) {
        if (state == PLAYING || state == HUB) {
            is_paused = !is_paused;

            if (is_paused) pause_music();
            else resume_music();
        }
    }

    double active_dt = is_paused ? 0.0 : dt;

    if (state == HUB) {
        update_hub(active_dt);
    }
    else if (state == PLAYING) {
        update_playing(active_dt);
    }
    else if (state == DEAD) {
        draw_death_screen(active_dt);
    }
    else if(state == VICTORY) {
        draw_victory_screen(active_dt);
    } else if (state == DRAFTING) {
        update_drafting();
    }else if (state == MAIN_MENU) {
        update_main_menu();
    }

    if (is_paused) {
        draw_pause_menu();
    }
}

void GameController::update_hub(double dt)
{
    if (current_track != TRACK_HUB) {
        play_music(music_named("HubMusic"), -1, 0.6f);
        current_track = TRACK_HUB;
    }

    player.set_floor_type(FLOOR_GRASS);
    // Draw the floor
    for (int x = -32 ; x < screen_width() + 32; x += 32) {
        for (int y = -32; y < screen_height() + 32; y +=32) {
            draw_bitmap(bitmap_named("Grass"), x, y);
        }
    }

    // ==========================================
    // --- THE WEAPON DROP ZONE ---
    // ==========================================

    hub_float_angle += 180 *dt;
    if (hub_float_angle >= 360) hub_float_angle -= 360;

    double y_offset = sine(hub_float_angle) * 8.0;

    point_2d sword_rack_pos = {screen_width() / 2.0 - 400.0, 250.0};
    point_2d bow_rack_pos = { screen_width() / 2.0 - 300.0, 250.0};
    rectangle player_hitbox = rectangle_from(player.get_coordinates().x - 20, player.get_coordinates().y - 20, 40, 40);

    // ==========================================
    // --- THE LEADERBOARD MONUMENT ---
    // ==========================================
    rectangle board_rect = rectangle_from(screen_width() / 2.0 + 300, 250, 80, 120);
    fill_rectangle(rgba_color(30, 30, 35, 255), board_rect); // A dark slate monument
    draw_rectangle(COLOR_GOLD, board_rect);
    draw_text("RECORDS", COLOR_GOLD, "MenuFont", 12, board_rect.x + 15, board_rect.y + 10);

    // Interaction Check
    if (rectangles_intersect(player_hitbox, board_rect)) {
        if (!is_viewing_leaderboard) {
            draw_text("Press 'E' to View Records", COLOR_YELLOW, "TextFont", 16, board_rect.x - 40, board_rect.y - 30);
        }
        
        if (key_typed(E_KEY)) {
            is_viewing_leaderboard = !is_viewing_leaderboard; // Toggle the UI
        }
    } else {
        is_viewing_leaderboard = false; // Close it automatically if they walk away
    }

    

    // ==========================================
    // --- RENDER THE WEAPONS ---
    // ==========================================
    
    // THE SWORD RACK
    if (player.get_weapon() != SWORD) {
        // Draw floating Sword
        draw_bitmap(bitmap_named("SwordSprite"), sword_rack_pos.x - 15, sword_rack_pos.y - 40 + y_offset);
    } else {
        // If holding the sword, show the rack is empty
        draw_text("(Equipped)", COLOR_DARK_GRAY, "MenuFont", 12, sword_rack_pos.x - 25, sword_rack_pos.y - 20);
    }

    // THE BOW RACK
    if (player.get_weapon() != BOW) {
        // Draw floating Bow
        draw_bitmap(bitmap_named("BowSprite"), bow_rack_pos.x - 15, bow_rack_pos.y - 40 + y_offset);
    } else {

        // If holding the bow, show the rack is empty
        draw_text("(Equipped)", COLOR_DARK_GRAY, "MenuFont", 12, bow_rack_pos.x - 25, bow_rack_pos.y - 20);
    }

    // ==========================================
    // --- INTERACTION LOGIC ---
    // ==========================================
    
    rectangle sword_hitbox = rectangle_from(sword_rack_pos.x - 30, sword_rack_pos.y - 30, 60, 60);
    rectangle bow_hitbox = rectangle_from(bow_rack_pos.x - 30, bow_rack_pos.y - 30, 60, 60);

    // Check Sword Pick-up
    if (rectangles_intersect(player_hitbox, sword_hitbox) && player.get_weapon() != SWORD) {
        draw_text("Press 'E' to take Sword", COLOR_YELLOW, "TextFont", 16, sword_rack_pos.x - 60, sword_rack_pos.y - 60);
        
        if (key_typed(E_KEY)) {
            player.equip_weapon(SWORD);
        }
    }

    // Check Bow Pick-up
    if (rectangles_intersect(player_hitbox, bow_hitbox) && player.get_weapon() != BOW) {
        draw_text("Press 'E' to take Bow", COLOR_YELLOW, "TextFont", 16, bow_rack_pos.x - 50, bow_rack_pos.y - 60);
        
        if (key_typed(E_KEY)) {
            player.equip_weapon(BOW);
        }
    }

    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt);

    // ==========================================
    // --- HUB PROJECTILE LOGIC ---
    // ==========================================

    point_2d target_pos;

    if (player.pull_trigger(target_pos)) {
        Projectile new_arrow(player.get_coordinates(), target_pos);
        new_arrow.set_is_friendly(true);
        projectiles.add(new_arrow);
    }

    for (int i = projectiles.length() - 1 ; i>= 0 ; --i) {
        projectiles[i].update(dt);
        projectiles[i].draw();

        point_2d pos = projectiles[i].get_coordinates();
        if (pos.x < 0 || pos.x > screen_width() || pos.y < 0 || pos.y > screen_height()) {
            projectiles.remove(i);
        }
    }
    

    draw_text("THE HUB", COLOR_WHITE, "MenuFont", 60, screen_width() / 2 - 120, 50);
    draw_text("Safe Zone", COLOR_LIGHT_GRAY, "TextFont", 20, screen_width() / 2 - 40, 120);

    // The Door to Tartarus
    rectangle tartarus_door = rectangle_from(screen_width() / 2 - 60, 180, 120, 80);
    fill_rectangle(COLOR_PURPLE, tartarus_door);
    draw_text("ENTER", COLOR_WHITE, "TitleFont", 20, tartarus_door.x + 25, tartarus_door.y + 30);

    player.draw();
    if (show_debug) {
        player.draw_hitbox();
    }

    player.draw_hud();
    
    
    if (rectangles_intersect(player_hitbox, tartarus_door)) {
        // Delegate graph generation and enemy spawning to the LevelManager
        level_manager.generate_graph();
        level_manager.spawn_wave(enemy_arr, screen_width(), screen_height());
        
        player.set_coordinates(100, screen_height() / 2); 
        state = PLAYING;
    }

    // Draw the UI Overlay if active
    if (is_viewing_leaderboard) {
        fill_rectangle(rgba_color(10, 10, 15, 230), 0, 0, screen_width(), screen_height());
        draw_text("H A L L   O F   H E R O E S", COLOR_GOLD, "TitleFont", 60, screen_width() / 2 - 280, 150);
        
        for (int i = 0; i < 5; ++i) {
            string rank = to_string(i + 1) + ".";
            string score_val = to_string(top_scores[i]);
            
            int y_pos = 280 + (i * 60);
            
            // Draw Rank
            draw_text(rank, COLOR_LIGHT_GRAY, "TitleFont", 35, screen_width() / 2 - 150, y_pos);
            // Draw Score
            draw_text(score_val, COLOR_WHITE, "TitleFont", 35, screen_width() / 2 + 50, y_pos);
        }
        
        draw_text("Press 'E' or Walk Away to Close", COLOR_GRAY, "MenuFont", 20, screen_width() / 2 - 140, screen_height() - 100);
    }
}

void GameController::update_playing(double dt)
{

    if (!is_paused) {
        run_timer += dt;
    if (!room_cleared && enemy_arr.length() > 0) {
        if (level_manager.get_current_room()->get_reward() == BOSS) {
            BossEnemy* boss = static_cast<BossEnemy*>(enemy_arr[0]);

            if (boss->get_is_enraged()) {
                if (current_track != TRACK_BOSS_2) {
                    play_music(music_named("BossMusic2"), -1, 0.8f);
                    current_track = TRACK_BOSS_2;
                }
            } else {
                if (current_track != TRACK_BOSS) {
                    play_music(music_named("BossMusic"), -1, 0.8f);
                    current_track = TRACK_BOSS;
                } else {
                    resume_music();
                }
            }
        } else {
            if (current_track != TRACK_BATTLE) {
                play_music(music_named("BattleMusic"), -1, 0.5f);
                current_track = TRACK_BATTLE;
            } else {
                resume_music();
            }
        }
    }
}
    player.set_floor_type(FLOOR_STONE);
    // Draw dungeon floor
    int tile_size = 64;

    for (int x = -tile_size; x < screen_width() + tile_size; x += tile_size) {
        for (int y = -tile_size; y < screen_height() + tile_size; y += tile_size) {
        
                draw_bitmap(bitmap_named("FloorBase"), x , y);
           
        }
    }

    int border_thickness = 60;
    color abyss_color = rgba_color(10, 10, 15, 255); // Deep, dark void
    
    // Draw the 4 massive rectangles blocking out the edges of the room
    fill_rectangle(abyss_color, 0, 0, screen_width(), border_thickness); // Top
    fill_rectangle(abyss_color, 0, screen_height() - border_thickness, screen_width(), border_thickness); // Bottom
    fill_rectangle(abyss_color, 0, 0, border_thickness, screen_height()); // Left
    fill_rectangle(abyss_color, screen_width() - border_thickness, 0, border_thickness, screen_height()); // Right

    // Draw a crisp silver trim around the edge of the floor so it pops!
    draw_rectangle(COLOR_LIGHT_GRAY, border_thickness, border_thickness, screen_width() - (border_thickness * 2), screen_height() - (border_thickness * 2));

    // ==========================================
    // --- THE ARENA CORRUPTION EFFECT ---
    // ==========================================

    bool arena_corrupted = false;

    for (int i = 0; i < enemy_arr.length(); i++) {
        if (enemy_arr[i]->get_is_enraged()) {
            arena_corrupted = true;
            break;
        }
    }

    if (arena_corrupted) {
        // 2. Tick the shared animation forward
        update_animation(fire_wall_anim);

        // 3. Bathe the entire room in a dark, oppressive crimson shadow
        fill_rectangle(rgba_color(40, 0, 10, 150), camera_x(), camera_y(), screen_width(), screen_height());

        // 4. Shrink the arena!
        if (arena_radius > 550.0) {
            arena_radius -= 100.0 * dt; 
        }

        // 5. Draw the Animated Fire Ring
        bitmap fire_bmp = bitmap_named("FireWall");
        double cell_w = bitmap_cell_width(fire_bmp); 
        
        double circumference = 2 * 3.14159 * arena_radius;
        int num_fires = circumference / (cell_w * 0.9); // Overlap slightly to prevent gaps

        drawing_options opts = option_with_animation(fire_wall_anim);

        for (int i = 0; i < num_fires; ++i) {
            double angle = (360.0 / num_fires) * i;
            
            double draw_x = arena_center.x + cosine(angle) * arena_radius - (cell_w / 2.0);
            double draw_y = arena_center.y + sine(angle) * arena_radius - (bitmap_cell_height(fire_bmp) / 2.0);
            
            draw_bitmap(fire_bmp, draw_x, draw_y, opts);
        }

        // 6. The Fire Wall Hitbox (Electric Fence)
        double player_dist = point_point_distance(player.get_coordinates(), arena_center);
        if (player_dist > arena_radius - 20) { 
            
            // Violent bounce back
            vector_2d bounce_dir = vector_point_to_point(player.get_coordinates(), arena_center);
            if (vector_magnitude(bounce_dir) > 0) {
                bounce_dir = unit_vector(bounce_dir);
                bounce_dir.x *= 1200;
                bounce_dir.y *= 1200;
                player.apply_knockback(bounce_dir, 0.4);
            }

        }

        // 7. Ambient Embers
        if (rnd() < 0.3) { 
            Particle ember;
            ember.position.x = camera_x() + rnd(screen_width());
            ember.position.y = camera_y() + screen_height() + 10; 
            ember.velocity.y = -50 - (rnd() * 100); 
            ember.velocity.x = (rnd() * 40) - 20;   
            ember.max_lifetime = 2.0 + rnd() * 2.0; 
            ember.lifetime = ember.max_lifetime;
            ember.base_color = rgba_color(255, 50, 0, 255); 
            ember.start_size = 4 + rnd() * 6;
            particles.add(ember);
        }
    }
    
    double real_dt = dt;

    if (hit_pause_timer > 0) {
        hit_pause_timer -= real_dt;
        dt = 0;
    }

    if (screen_shake_timer > 0) {
        screen_shake_timer -= real_dt;
        move_camera_to(rnd(10) - 5, rnd(10) - 5);
    } else {
        move_camera_to(0, 0);
    }

    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt);

    // ZEUS DASH
    if (player.get_and_clear_dash_event()) {
        if (player.has_boon(BOON_ZEUS_DASH)) {
            for (int p = 0; p < 15; p++) {
                Particle spark;
                spark.base_color = COLOR_YELLOW;
                spark.position = player.get_coordinates();
                double angle = (360.0 / 15) * p;
                double speed = 600 + (rnd() * 300); 
                spark.velocity.x = cosine(angle) * speed;
                spark.velocity.y = sine(angle) * speed;
                spark.max_lifetime = 0.2;
                spark.lifetime = spark.max_lifetime;
                spark.start_size = 10;
                particles.add(spark);
            }

            for (int i = 0; i < enemy_arr.length(); i++) {
                if (point_point_distance(player.get_coordinates(), enemy_arr[i]->get_coordinates()) <= 250) {
                    
                    // Base 30 Lightning Damage
                    enemy_arr[i]->take_damage(30); 
                    
                    // Violent stun & knockback to clear space
                    vector_2d kb = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());
                    if (vector_magnitude(kb) > 0) {
                        kb = unit_vector(kb);
                        kb.x *= 700;
                        kb.y *= 700;
                        enemy_arr[i]->apply_knockback(kb, 0.3);
                    }

                    // Strike particles on the enemy
                    for (int p = 0; p < 5; ++p) {
                        Particle bolt;
                        bolt.base_color = COLOR_WHITE;
                        bolt.position = enemy_arr[i]->get_coordinates();
                        bolt.velocity.x = (rnd() * 1000) - 500;
                        bolt.velocity.y = (rnd() * 1000) - 500;
                        bolt.max_lifetime = 0.15;
                        bolt.lifetime = bolt.max_lifetime;
                        particles.add(bolt);
                    }
            }
        }
    }
}

    // ==========================================
    // --- FIRE BOW LOGIC ---
    // ==========================================
    point_2d target_pos;
    if (player.pull_trigger(target_pos)) {

        // 3. Create the projectile (adjust this line to match your exact constructor!)
        Projectile new_arrow(player.get_coordinates(), target_pos);
        
        // 4. FLAG IT AS FRIENDLY!
        new_arrow.set_is_friendly(true); 

        // 5. Add it to the array
        projectiles.add(new_arrow);
    }
    // ==========================================
    // --- DEVELOPER CHEATS ---
    // ==========================================
    if (key_typed(K_KEY)) {
        for (int i = 0; i < enemy_arr.length(); i++) {
            delete enemy_arr[i];
        }
        enemy_arr.clear();
    }
    if (key_typed(G_KEY)) {
        player.heal(99999);
    }

    if (key_typed(B_KEY)) {
        // 1. Wipe out any existing enemies in the room
        for (int i = 0; i < enemy_arr.length(); i++) {
            delete enemy_arr[i];
        }
        enemy_arr.clear();
        
        // 2. Spawn the Titan perfectly in the center! (99 is just a placeholder ID)
        enemy_arr.add(new BossEnemy(screen_width() / 2, screen_height() / 2, 99));
    }

    if (key_typed(H_KEY)) {
        player.increase_damage(5000);
    }

    if (key_typed(P_KEY)) {
        show_debug = !show_debug;
    }

    if (combo_timer > 0) {
        combo_timer -= dt;
        if (combo_timer <= 0) {
            combo_multiplier = 1;
        }
    }

    // --- SWORD LOGIC ---
    if (player.is_hitbox_active()) {
        point_2d sword_center = player.get_attack_hitbox();

        for (int i = enemy_arr.length() - 1; i >= 0; --i) {
             if (point_point_distance(enemy_arr[i]->get_coordinates(), sword_center) <= 50 + enemy_arr[i]->get_collision_radius()) {
                if (!player.has_hit(enemy_arr[i]->get_id())) {
                    current_score += (10 * combo_multiplier);
                    combo_multiplier++;
                    if (combo_multiplier > 10) combo_multiplier = 10;
                    combo_timer = 2.5;
                    
                    play_sound_effect("Impact", 1, 0.7);
                    enemy_arr[i]->take_damage(player.get_sword_damage());
                    player.add_to_hitlist(enemy_arr[i]->get_id());

                    // ==========================================
                    // --- NEW: ZEUS CHAIN LIGHTNING ---
                    // ==========================================
                    if (player.has_boon(BOON_ZEUS_STRIKE)) {
                        for (int j = 0; j < enemy_arr.length(); ++j) {
                            if (i != j && point_point_distance(enemy_arr[i]->get_coordinates(), enemy_arr[j]->get_coordinates()) <= 100) {
                                enemy_arr[j]->take_damage(15);

                                for (int p = 0; p < 5; ++p) {
                                    Particle spark;
                                    spark.base_color = COLOR_YELLOW;
                                    spark.position = enemy_arr[j]->get_coordinates();
                                    spark.velocity.x = (rnd() * 1000) - 500;
                                    spark.velocity.y = (rnd() * 1000) - 500;
                                    spark.max_lifetime = 0.15;
                                    spark.lifetime = spark.max_lifetime;
                                    particles.add(spark);
                                }
                            }
                        }
                    }
                    
                    // Check if this is the heavy 3rd strike!
                    bool is_heavy_hit = (player.get_combo_mult() >= 2.0);

                    vector_2d opposite_vec = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());

                    if (vector_magnitude(opposite_vec) > 0) {
                        vector_2d knockback = unit_vector(opposite_vec);
                        
                        // Double the knockback force and stun duration for the heavy finisher!
                        if (is_heavy_hit) {
                            knockback.x *= 1500;
                            knockback.y *= 1500;
                            enemy_arr[i]->apply_knockback(knockback, 0.7);


                            for (int j = 0; j < enemy_arr.length(); ++j) {
                                // If it's a different enemy, and they are within 120 pixels of the impact:
                                if (i != j && point_point_distance(enemy_arr[i]->get_coordinates(), enemy_arr[j]->get_coordinates()) <= 120) {
                                    
                                    // 1. Deal splash damage (half of the heavy attack's damage)
                                    enemy_arr[j]->take_damage(player.get_sword_damage() * 0.5);

                                    // 2. Blast them away from the epicenter of the explosion!
                                    vector_2d splash_kb = vector_point_to_point(enemy_arr[i]->get_coordinates(), enemy_arr[j]->get_coordinates());
                                    if (vector_magnitude(splash_kb) > 0) {
                                        splash_kb = unit_vector(splash_kb);
                                        splash_kb.x *= 900; // Strong radial knockback
                                        splash_kb.y *= 900;
                                        enemy_arr[j]->apply_knockback(splash_kb, 0.5);
                                    }
                                }
                            }
                            for (int p = 0; p < 15; ++p) {
                                Particle blast;
                                blast.base_color = COLOR_WHITE;
                                if (rnd() < 0.5) blast.base_color = COLOR_LIGHT_GRAY;
                                
                                blast.position = enemy_arr[i]->get_coordinates();
                                
                                // Explode outward in a perfect 360 circle
                                double angle = (360.0 / 15) * p;
                                double speed = 400 + (rnd() * 400); 
                                blast.velocity.x = cosine(angle) * speed;
                                blast.velocity.y = sine(angle) * speed;
                                
                                blast.max_lifetime = 0.25;
                                blast.lifetime = blast.max_lifetime;
                                blast.start_size = 10 + rnd(10);
                                particles.add(blast);
                            }
                        } else {
                            knockback.x *= 700;
                            knockback.y *= 700;
                            enemy_arr[i]->apply_knockback(knockback, 0.4);
                        }
                    }

                    // --- DYNAMIC HIT-STOP & SCREEN SHAKE ---
                    // Freeze the game for 0.12s on a heavy hit to simulate the blade "catching"
                    double target_hitstop = is_heavy_hit ? 0.12 : 0.05;
                    if (hit_pause_timer < target_hitstop) {
                        hit_pause_timer = target_hitstop;
                    }
                    
                    // Violent camera vibration
                    double target_shake = is_heavy_hit ? 0.3 : 0.1;
                    if (screen_shake_timer < target_shake) {
                        screen_shake_timer = target_shake;
                    }
                }    
            }
        }
    }

    // --- PROJECTILE LOGIC ---
    for (int i = projectiles.length() - 1; i >= 0; --i) {
        point_2d pos = projectiles[i].get_coordinates();


        if (!projectiles[i].get_is_deflected() && !projectiles[i].get_is_friendly() && point_point_distance(pos, player.get_coordinates()) <= 25) {

            if (player.get_is_parrying()) {
                current_score += (25 * combo_multiplier);
                combo_multiplier++;
                if (combo_multiplier > 10) combo_multiplier = 10;
                combo_timer = 2.5;
                hit_pause_timer = 0.1;
                screen_shake_timer = 0.15;
                play_sound_effect("Parry");

                vector_2d arrow_kb = vector_point_to_point(pos, player.get_coordinates());
                if (vector_magnitude(arrow_kb) > 0) {
                    arrow_kb = unit_vector(arrow_kb);
                    arrow_kb.x *= 400;
                    arrow_kb.y *= 400;

                    player.apply_knockback(arrow_kb, 0.25);
                }

                projectiles[i].deflect();
                continue;
            }

            else if (!player.is_invincible()) {
                combo_multiplier = 1;
                combo_timer = 0;

                player.take_damage(20);
                screen_shake_timer = 0.1;
            }
            projectiles.remove(i);
            continue;
        }

        if (pos.x < 0 || pos.x > screen_width() || pos.y < 0 || pos.y > screen_height()) {
            projectiles.remove(i);
            continue;
        }
    }

    // --- ENEMY LOOP & PARTICLES ---
    for (int i = enemy_arr.length() - 1; i >= 0; i--) {
        if (enemy_arr[i]->get_health() <= 0) {

            // Kamikaze

            if (enemy_arr[i]->has_exploded()) {
                DangerZone dz;
                dz.position = enemy_arr[i]->get_coordinates();
                dz.radius = 100;
                dz.timer = 0;
                danger_zones.add(dz);
            }

            for (int p = 0; p < 8; ++p) {
                Particle debris;
                debris.base_color = COLOR_LIGHT_GRAY;
                debris.position = enemy_arr[i]->get_coordinates();
                debris.velocity.x = (rnd() * 800) - 400;
                debris.velocity.y = (rnd() * 800) - 400;
                debris.max_lifetime = 0.2 + (rnd() * 0.3);
                debris.lifetime = debris.max_lifetime;
                particles.add(debris);
            }

            delete enemy_arr[i];
            enemy_arr.remove(i);
            continue;
        }
        enemy_arr[i]->update(player, dt, projectiles, particles);
        enemy_arr[i]->draw(show_debug);

        // BOSS AOE
        double aoe_radius = 0;
        if (enemy_arr[i]->check_and_clear_aoe(aoe_radius)) {
            DangerZone dz;
            dz.position = enemy_arr[i]->get_coordinates();
            dz.radius = aoe_radius;
            dz.timer = 0;
            danger_zones.add(dz);
        }

        // ENEMY VS DEFLECTED PROJECTILE COLLISION
        for (int j = projectiles.length() - 1; j >= 0; --j) {
            if (projectiles[j].get_is_deflected() || projectiles[j].get_is_friendly()) {
                if (point_point_distance(enemy_arr[i]->get_coordinates(), projectiles[j].get_coordinates()) <= 10 + enemy_arr[i]->get_collision_radius()) {
                    play_sound_effect("Impact", 1, 0.7);
                    enemy_arr[i]->take_damage(70);

                    vector_2d kb = vector_point_to_point(projectiles[j].get_coordinates(), enemy_arr[i]->get_coordinates());

                    if (vector_magnitude(kb) > 0) {
                        kb = unit_vector(kb);
                        kb.x *= 600;
                        kb.y *= 600;
                        enemy_arr[i]->apply_knockback(kb, 0.4);
                    }   
                    projectiles.remove(j);
                }
            }
        }

        // MELEE COLLISION
        if (enemy_arr[i]->is_stunned()) continue;
        double dist = point_point_distance(enemy_arr[i]->get_coordinates(), player.get_coordinates());

        double damage_threshold = enemy_arr[i]->get_damage_radius() + 20.0; 

        // 1. CHECK PARRY FIRST (With a 25px forgiveness buffer!)
        if (dist <= damage_threshold + 25.0 && player.get_is_parrying() && !player.get_parry_landed()) {
            
            player.set_parry_landed(true);
            // Grant score
            current_score += (50 * combo_multiplier);
            combo_multiplier++;
            if (combo_multiplier > 10) combo_multiplier = 10;
            combo_timer = 2.5;

            player.grant_iframes(0.7);
            player.reset_parry_cooldown();
         

            hit_pause_timer = 0.15;
            screen_shake_timer = 0.2;
            play_sound_effect("Parry");

            vector_2d player_slide = vector_point_to_point(enemy_arr[i]->get_coordinates(), player.get_coordinates());
            if (vector_magnitude(player_slide) > 0) {
                player_slide = unit_vector(player_slide);
                player_slide.x *= 1000;
                player_slide.y *= 1000;
                player.apply_knockback(player_slide, 0.2);
            }

            vector_2d parry_force = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());
            if (vector_magnitude(parry_force) > 0) {
                parry_force = unit_vector(parry_force);
                parry_force.x *= 1000; 
                parry_force.y *= 1000;
                
                enemy_arr[i]->apply_knockback(parry_force, 1.0); 
            }
        }
        // 2. CHECK DAMAGE SECOND (Strict physical collision)
        else if (dist <= damage_threshold && !player.is_invincible()) {
            combo_multiplier = 1;
            combo_timer = 0;

            player.take_damage(20);
            screen_shake_timer = 0.2; 
            hit_pause_timer = 0.15;   

            vector_2d knockback = vector_point_to_point(enemy_arr[i]->get_coordinates(), player.get_coordinates());
            if (vector_magnitude(knockback) > 0) {
                vector_2d kb_dir = unit_vector(knockback);
                kb_dir.x *= 900;
                kb_dir.y *= 900;
                player.apply_knockback(kb_dir, 0.4);
            }

            vector_2d raw_enemy_kb = vector_point_to_point(player.get_coordinates(), enemy_arr[i]->get_coordinates());
            if (vector_magnitude(raw_enemy_kb) > 0) {
                vector_2d enemy_kb_dir = unit_vector(raw_enemy_kb);
                enemy_kb_dir.x *= 300;
                enemy_kb_dir.y *= 300;
                enemy_arr[i]->apply_knockback(enemy_kb_dir, 0.6);
            }
        }
    }
        
    for (int i = projectiles.length() - 1 ; i >= 0; i--) {
        projectiles[i].update(dt);

        // --- ADD THIS SPARK TRAIL ---
        if (projectiles[i].get_is_deflected()) {
            Particle trail_spark;
            trail_spark.base_color = COLOR_GOLD;
            trail_spark.position = projectiles[i].get_coordinates();
            // Shoot the sparks in random directions slightly
            trail_spark.velocity.x = (rnd() * 200) - 100;
            trail_spark.velocity.y = (rnd() * 200) - 100;
            trail_spark.max_lifetime = 0.2; 
            trail_spark.lifetime = trail_spark.max_lifetime;
            particles.add(trail_spark);
        }
        projectiles[i].draw();
    }

    // --- PARTICLE PHYSICS & RENDER LOOP ---
    for (int i = particles.length() - 1; i >= 0; --i) {
        particles[i].position.x += particles[i].velocity.x * dt;
        particles[i].position.y += particles[i].velocity.y * dt;

        particles[i].velocity.x -= particles[i].velocity.x * 12.0 * dt;
        particles[i].velocity.y -= particles[i].velocity.y * 12.0 * dt;

        particles[i].lifetime -= dt;
        if (particles[i].lifetime <= 0) {
            particles.remove(i);
        } else {
            double size_ratio = particles[i].lifetime / particles[i].max_lifetime;
            int current_size = particles[i].start_size * size_ratio;

            int alpha = 255 * size_ratio;
            if (alpha  < 0) alpha = 0;
            
            color final_color= rgba_color(
                red_of(particles[i].base_color), 
                green_of(particles[i].base_color), 
                blue_of(particles[i].base_color), 
                alpha
            ); 
            fill_rectangle(final_color, particles[i].position.x, particles[i].position.y, current_size, current_size);
        }
    }

    player.draw();

    if (show_debug) {
        player.draw_hitbox();
        draw_circle(COLOR_RED, player.get_coordinates().x, player.get_coordinates().y, 25); 
    
    // This is the 50px radius where projectiles can hurt you
    draw_circle(COLOR_BLUE, player.get_coordinates().x , player.get_coordinates().y, 20);
    }


    // ==========================================
    // --- ROOM CLEAR AND BUBBLE LOGIC ---
    // ==========================================
    if (enemy_arr.length() == 0) {

        if (level_manager.get_current_room()->get_reward() == BOSS) {
            state = VICTORY;
            stop_music();
            current_track = TRACK_NONE;
            return;
        }

        if (!current_reward.is_active && !room_cleared) {
            pause_music();
            projectiles.clear(); 
            current_reward.is_active = true;
            current_reward.position.x = screen_width() / 2;
            current_reward.position.y = screen_height() / 2;
            current_reward.type = level_manager.get_current_room()->get_reward();
        }
    }

    if (current_reward.is_active) {
        color drop_color = COLOR_WHITE;
        switch (current_reward.type) {
            case HEAL:
                drop_color = COLOR_GREEN;
                break;
            
            case ARES:
                drop_color = COLOR_RED;
                break;

            case ZEUS:
                drop_color = COLOR_YELLOW;
                break;

            case BOSS:
                break;
        }
        fill_circle(drop_color, current_reward.position.x, current_reward.position.y, 25);
        draw_circle(COLOR_WHITE, current_reward.position.x, current_reward.position.y, 27);

        if (point_point_distance(player.get_coordinates(), current_reward.position) <= 40) {
            current_reward.is_active = false;
            room_cleared = true;

            if (current_reward.type == HEAL) {
                player.heal(100);
                level_manager.generate_doors(screen_width(), screen_height());
            }
            else if (current_reward.type == ARES || current_reward.type == ZEUS) {
                populate_draft_menu(current_reward.type);
                state = DRAFTING;
            }
        }
    }

    dynamic_array<PhysicalDoor>& active_doors = level_manager.get_active_doors();
    
    for (int i = 0; i < active_doors.length(); i++) {
        fill_rectangle(COLOR_BLACK, active_doors[i].bounds);
        
        string reward_text = reward_to_string(active_doors[i].destination->get_reward());
        color symbol_color = COLOR_WHITE;
        
        // Add color coding to your doors!
        if (active_doors[i].destination->get_reward() == HEAL) symbol_color = COLOR_GREEN;
        if (active_doors[i].destination->get_reward() == ARES) symbol_color = COLOR_RED;
        if (active_doors[i].destination->get_reward() == ZEUS) symbol_color = COLOR_YELLOW;

        draw_text(reward_text, symbol_color, "TitleFont", 20, active_doors[i].bounds.x - 20, active_doors[i].bounds.y - 30);

        rectangle player_hitbox = rectangle_from(player.get_coordinates().x - 20, player.get_coordinates().y - 20, 40, 40);
        
        if (rectangles_intersect(player_hitbox, active_doors[i].bounds)) {
            level_manager.set_current_room(active_doors[i].destination);
            level_manager.clear_doors();
            level_manager.spawn_wave(enemy_arr, screen_width(), screen_height());
            
            player.set_coordinates(100, screen_height() / 2);
            
            // --- CRITICAL: Reset flags for the next room! ---
            room_cleared = false; 
            current_reward.is_active = false;
            break; 
        }
    }

    // ==========================================
    // --- THE DANGER ZONE (DETONATION) LOOP ---
    // ==========================================
    for (int i = danger_zones.length() - 1; i >= 0; --i) {
        danger_zones[i].timer -= dt;

        // Since the Kamikaze passes a 0.0 timer, this triggers instantly!
        if (danger_zones[i].timer <= 0) {
            play_sound_effect("Explosion", 1, 0.8f);
            screen_shake_timer = 0.3; // Violent screen shake
            hit_pause_timer = 0.1;
            
            // Did the player get caught inside the 150px radius?
            if (point_point_distance(player.get_coordinates(), danger_zones[i].position) <= danger_zones[i].radius) {
                if (!player.is_invincible()) {
                    combo_timer = 0;
                    combo_multiplier = 1;
                    player.take_damage(40); 
                    
                    // Violent knockback out of the explosion center
                    vector_2d kb = vector_point_to_point(danger_zones[i].position, player.get_coordinates());
                    if (vector_magnitude(kb) > 0) {
                        kb = unit_vector(kb);
                        kb.x *= 1500;
                        kb.y *= 1500;
                        player.apply_knockback(kb, 0.5);
                    }
                }
            }
            // Remove the bomb from memory now that it has exploded
            danger_zones.remove(i);
        }
    }
        
    player.draw_hud();

    // ==========================================
    // --- ARCADE SCORE HUD (TOP RIGHT) ---
    // ==========================================
    
    double score_x = camera_x() + screen_width() - 250; 
    double score_y = camera_y() + 30;

    // 1. Draw the Base Score
    draw_text("SCORE: " + to_string(current_score), COLOR_WHITE, "TitleFont", 30, score_x, score_y);
    int minutes = (int) run_timer / 60;
    int seconds = (int) run_timer % 60;
    string time_str = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
    draw_text("TIME: " + time_str, COLOR_LIGHT_GRAY, "MenuFont", 24, score_x, score_y + 35);

    // 2. Draw the Flashy Combo Meter (Only if combo is active!)
    if (combo_multiplier > 1) {
        
        string combo_text = "x" + to_string(combo_multiplier) + " COMBO!";
        
        // Colors get more intense as the combo grows
        color combo_color = COLOR_YELLOW;
        if (combo_multiplier >= 5) combo_color = COLOR_ORANGE;
        if (combo_multiplier >= 10) combo_color = COLOR_RED; 

        // Add a slight jitter effect if you hit the max 10x combo!
        double jitter_x = (combo_multiplier >= 10) ? (rnd(4) - 2) : 0;
        double jitter_y = (combo_multiplier >= 10) ? (rnd(4) - 2) : 0;

        draw_text(combo_text, combo_color, "TitleFont", 24, score_x + 50 + jitter_x, score_y + 70 + jitter_y);

        // 3. Draw the Combo Timer Bar
       
        double timer_percent = combo_timer / 2.5; 
        if (timer_percent < 0) timer_percent = 0;

        // A dark grey background bar
        fill_rectangle(rgba_color(50, 50, 50, 200), score_x + 50, score_y + 65, 120, 8);
        
        // The shrinking colored timer bar over top
        fill_rectangle(combo_color, score_x + 50, score_y + 65, 120 * timer_percent, 8);
    }
        
    if (player.is_dead()) {
        state = DEAD;
        stop_music();
        current_track = TRACK_NONE;
    }
}

void GameController::draw_death_screen(double dt)
{
    if (current_track != TRACK_DEFEAT) {
        play_music(music_named("DefeatMusic"), 1, 0.8f);
        current_track = TRACK_DEFEAT;
    }
    death_timer += dt;
    fill_rectangle(rgba_color(20, 0, 0, 210), 0, 0, screen_width(), screen_height());

    if (death_timer > 1.5) {
        double fade_progress = (death_timer - 1.5) / 2.0;
        if (fade_progress > 1.0) fade_progress = 1.0;

        int alpha = 255 * fade_progress; // Convert progress to alpha value
        draw_text("Y O U   D I E D", rgba_color(180, 0, 0, alpha), "TitleFont", 110, screen_width() / 2 - 320, screen_height() / 2 - 100);
        
        draw_text("The Abyss consumes another soul.", rgba_color(150, 150, 150, alpha), "TextFont", 30, screen_width() / 2 - 210, screen_height() / 2 + 10);
    }

    if (death_timer > 3.5) {
        int pulse_alpha = 150 + (sine(death_timer * 150) * 100);
        draw_text("Press 'R' to Surrender and Return", rgba_color(200, 200, 200, pulse_alpha), "MenuFont", 24, screen_width() / 2 - 195, screen_height() / 2 + 120);

         if (key_typed(R_KEY)) {
            save_new_score(current_score);
            current_score = 0;
            combo_multiplier = 1;
            reset();
        }
    }
    

   
}

void GameController::draw_victory_screen(double dt)
{
    // Tick the timer forward!
    victory_timer += dt;

    // --- PHASE 1: AUDIO SWELL ---
    if (current_track != TRACK_VICTORY) {
        play_music(music_named("VictoryMusic"), 1, 0.8f);
        current_track = TRACK_VICTORY;
    }

    if (screen_shake_timer > 0) {
        screen_shake_timer -= dt;
        move_camera_to(rnd(10) - 5, rnd(10) - 5);
    } else {
        move_camera_to(0, 0);
    }

    // --- THE MATH ---
    if (!time_bonus_applied) {
        int time_bonus = 10000 - ((int)run_timer * 20);
        if (time_bonus < 0) time_bonus = 0;

        current_score += time_bonus;
        time_bonus_applied = true;
    }

    int minutes = (int)run_timer / 60;
    int seconds = (int)run_timer % 60;
    string final_time = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
    
    int display_bonus = 10000 - ((int)run_timer * 20);
    if (display_bonus < 0) display_bonus = 0;
    int stats_y = screen_height() / 2 + 30;

    // --- PHASE 2: THE ATMOSPHERE (Instant) ---
    fill_rectangle(rgba_color(0, 0, 0, 180), 0, 0, screen_width(), screen_height());

    // --- PHASE 3: THE GOLDEN TITLE (Fades in over 1.5 seconds) ---
    if (victory_timer > 0.5) {
        double progress = (victory_timer - 0.5) / 1.5;
        if (progress > 1.0) progress = 1.0;
        int alpha = 255 * progress;

        draw_text("V I C T O R Y", rgba_color(255, 215, 0, alpha), "MenuFont", 110, camera_x() + screen_width() / 2 - 340, camera_y() + screen_height() / 2 - 150);
    }

    // --- PHASE 4: THE SUBTITLE & STATS (Pops in at 2.5s) ---
    if (victory_timer > 2.5) {
        draw_text("The Titan has fallen", COLOR_WHITE, "TextFont", 30, screen_width() / 2 - 150, screen_height() / 2 - 20);
        draw_text("CLEAR TIME: " + final_time, COLOR_LIGHT_GRAY, "MenuFont", 24, screen_width() / 2 - 120, stats_y);
        draw_text("TIME BONUS: +" + to_string(display_bonus), COLOR_CYAN, "MenuFont", 24, screen_width() / 2 - 120, stats_y + 30);
    }

    // --- PHASE 5: THE FINAL SCORE SLAM (Explodes in at 4.0s) ---
    if (victory_timer > 4.0) {
        
        // Trigger the bass hit exactly ONCE!
        // --- NEW: THE SCORE COUNT-UP MATH ---
        // Ticks up over exactly 1.5 seconds
        double count_progress = (victory_timer - 4.0) / 1.5; 
        if (count_progress > 1.0) count_progress = 1.0;
        
        int display_score = current_score * count_progress;
        
        // Flicker the text red while counting, snap to solid yellow when done!
        color score_col = COLOR_YELLOW;
        if (count_progress < 1.0) {
            // Flickers every few frames
            if ((int)(victory_timer * 20) % 2 == 0) score_col = COLOR_RED; 
            else score_col = COLOR_WHITE;
            
            // Add a ticking sound using your light sword swing!
            if ((int)(victory_timer * 20) % 3 == 0) play_sound_effect("Sword", 1, 0.1f); 
        }

        draw_text("FINAL SCORE: " + to_string(display_score), score_col, "TitleFont", 35, camera_x() + screen_width() / 2 - 140, stats_y + 80);
    }

    // --- PHASE 6: THE INPUT (Pulses at 6.0s) ---
    if (victory_timer > 6.0) {
        int pulse = 150 + (sine(victory_timer * 150) * 100);
        draw_text("Press 'R' to Return to Hub", rgba_color(200, 200, 200, pulse), "MenuFont", 30, screen_width() / 2 - 190, screen_height() / 2 + 180);

        if (key_typed(R_KEY)) {
            save_new_score(current_score);
            reset();
        }
    }
}

void GameController::reset()
{
    for (int i = 0; i < enemy_arr.length(); i++) {
        delete enemy_arr[i];
    }
    
    enemy_arr.clear();
    projectiles.clear();
    particles.clear();
    danger_zones.clear();
    victory_timer = 0;
    score_sound_played = false;

    player.reset_stats();
    death_timer = 0;
    current_score = 0;
    combo_multiplier = 1;
    run_timer = 0;
    time_bonus_applied = false;
    hit_pause_timer = 0;
    screen_shake_timer = 0;
    arena_radius = 1200.0;

    // Send the player back to the safe zone
    state = HUB;
    player.set_coordinates(screen_width() / 2, screen_height() - 100); 
}

string GameController::reward_to_string(reward_type r) {
    if (r == HEAL)   return "HEAL +100 HP";
    if (r == ZEUS)  return "SPEED BOOST";
    if (r == ARES) return "DAMAGE BOOST";
    if (r == BOSS)   return "BOSS ROOM";
    return "???";
}

void GameController::apply_reward(reward_type reward)
{
    if (reward == HEAL) {
        player.heal(100);
    }
    else if (reward == ZEUS) {
        player.increase_speed(50);
    }
    else if (reward == ARES) {
        player.increase_damage(50);
    }
}

void GameController::populate_draft_menu(reward_type god_type)
{
    current_draft_options.clear();

    if (god_type == ARES) { 
        // Ares Theme: High raw damage, violent speed
        current_draft_options.add({BOON_ARES_STRIKE, "Blood Strike", "+40 Sword Damage.", COLOR_DARK_RED});
        current_draft_options.add({BOON_ARES_SWIFTNESS, "War Frenzy", "+150 Movement Speed.", COLOR_RED});
    } 
    else if (god_type == ZEUS) { 
        // Zeus Theme: Lower raw damage, but extreme speed and chain lightning
        current_draft_options.add({BOON_ZEUS_STRIKE, "Lightning Strike", "+15 Damage & Sparks.", COLOR_YELLOW});
        current_draft_options.add({BOON_ZEUS_SWIFTNESS, "Tailwind", "+250 Movement Speed.", COLOR_LIGHT_BLUE});
        current_draft_options.add({BOON_ZEUS_DASH, "Thunder Dash", "Dash strikes nearby.", COLOR_WHITE});
    }
}

void GameController::update_drafting()
{
    // 1. Draw a dark semi-transparent overlay over the paused game
    fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, screen_width(), screen_height());
    string title_txt = "CHOOSE YOUR BLESSING";
    font t_font = font_named("TitleFont");
    double title_w = text_width(title_txt, t_font, 60);
    draw_text(title_txt, COLOR_WHITE, t_font, 60, (screen_width() / 2.0) - (title_w / 2.0), 100);

    // 2. Draw 3 Boon Cards side-by-side
    int card_width = 250;
    int card_height = 350;
    int start_x = (screen_width() - (card_width * 3 + 100)) / 2; // Center them perfectly

    for (int i = 0; i < current_draft_options.length(); i++) {
        int x = start_x + (i * (card_width + 50));
        int y = 250;
        rectangle card_rect = rectangle_from(x, y, card_width, card_height);

        // Draw the card background and colored border
        fill_rectangle(rgba_color(30, 30, 30, 255), card_rect);
        draw_rectangle(current_draft_options[i].theme, card_rect);

        // Draw the Boon text
        draw_text(current_draft_options[i].name, current_draft_options[i].theme, "TitleFont", 24, x + 20, y + 20);
        draw_text(current_draft_options[i].description, COLOR_WHITE, "TextFont", 16, x + 20, y + 80);

        // 3. Selection Logic (Did the player click this card?)
        if (mouse_clicked(LEFT_BUTTON)) {
            if (point_in_rectangle(mouse_position(), card_rect)) {
                
                // Give the player the boon!
                player.add_boon(current_draft_options[i]);

                // Spawn the exit doors NOW that a choice was made!
                level_manager.generate_doors(screen_width(), screen_height());
                
                state = PLAYING; // Unpause the game!
            }
        }
    }
}

void GameController::update_main_menu()
{
    // 1. THE ATMOSPHERE (Deep Blood Red / Obsidian Black)
    fill_rectangle(rgba_color(15, 5, 5, 255), 0, 0, screen_width(), screen_height());



    // 3. THE TITLE
    // Using COLOR_CRIMSON and spreading the letters out makes it look majestic
    draw_text("P R O J E C T   T I T A N", COLOR_CRIMSON, "MenuFont", 80, 100, 150);

    // ==========================================
    // --- ELEGANT HOVER BUTTONS ---
    // ==========================================
    
    // We use invisible rectangles just for the math, but we don't draw them!
    rectangle start_rect = rectangle_from(100, 350, 300, 50);
    rectangle quit_rect = rectangle_from(100, 450, 300, 50);

    // Check where the mouse is
    bool hover_start = point_in_rectangle(mouse_position(), start_rect);
    bool hover_quit = point_in_rectangle(mouse_position(), quit_rect);

    // If hovered, the text turns GOLD. If not, it stays a dark grey/white.
    color start_color = hover_start ? COLOR_GOLD : COLOR_LIGHT_GRAY;
    color quit_color = hover_quit ? COLOR_GOLD : COLOR_LIGHT_GRAY;

    // Draw the text (Notice the spaces between letters for that cinematic look!)
    draw_text("B E G I N   E S C A P E", start_color, "TitleFont", 35, start_rect.x, start_rect.y);
    draw_text("S U R R E N D E R", quit_color, "TitleFont", 35, quit_rect.x, quit_rect.y);

    // ==========================================
    // --- CLICK LOGIC ---
    // ==========================================
    if (mouse_clicked(LEFT_BUTTON)) {
        if (hover_start) {
            player.reset_stats(); 
            state = HUB; 
        }
        else if (hover_quit) {
            // A quick hack to force the game to close from inside the controller!
            exit(0); 
        }
    }
}

void GameController::draw_pause_menu()
{
    // 1. Darken the screen behind the menu
    fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, screen_width(), screen_height());

    // 2. Draw the Title
    draw_text("P A U S E D", COLOR_WHITE, "TitleFont", 70, screen_width() / 2 - 200, 200);

    // 3. Setup the Invisible Button Zones
    rectangle resume_btn = rectangle_from(screen_width() / 2 - 150, 350, 300, 50);
    rectangle main_btn = rectangle_from(screen_width() / 2 - 150, 430, 300, 50);
    rectangle quit_btn = rectangle_from(screen_width() / 2 - 150, 510, 300, 50);

    bool hover_res = point_in_rectangle(mouse_position(), resume_btn);
    bool hover_main = point_in_rectangle(mouse_position(), main_btn);
    bool hover_quit = point_in_rectangle(mouse_position(), quit_btn);

    // 5. Draw the Glowing Text
    draw_text("R E S U M E", hover_res ? COLOR_GOLD : COLOR_LIGHT_GRAY, "MenuFont", 35, resume_btn.x + 35, resume_btn.y);
    draw_text("M A I N   M E N U", hover_main ? COLOR_GOLD : COLOR_LIGHT_GRAY, "MenuFont", 35, main_btn.x, main_btn.y);
    draw_text("Q U I T   G A M E", hover_quit ? COLOR_GOLD : COLOR_LIGHT_GRAY, "MenuFont", 35, quit_btn.x + 10, quit_btn.y);

    // 6. Click Logic
    if (mouse_clicked(LEFT_BUTTON)) {
        if (hover_res) {
            is_paused = false;
            resume_music();
    
        } 
        else if (hover_main) {
            // Return to Main Menu safely!
            is_paused = false;
            state = MAIN_MENU;
            stop_music();
            current_track = TRACK_NONE;
        } 
        else if (hover_quit) {
            exit(0);
        }
    }
}

void GameController::load_leaderboard()
{
    std::ifstream file("leaderboard.txt");

    if (file.is_open()) {
        for (int i = 0; i < 5; i++) {
            file >> top_scores[i];
        }
        file.close();
    } else {

    }
}

void GameController::save_new_score(int final_score)
{
    int insert_index = -1;

    // Check if new score deserves to be on the board
    for (int i = 0; i < 5; ++i) {
        if (final_score > top_scores[i]) {
            insert_index = i;
            break;
        }
    }

    if (insert_index != -1) {
        for (int i = 4; i > insert_index; --i) {
            top_scores[i] = top_scores[i - 1];
        }
        top_scores[insert_index] = final_score;

        std::ofstream file("leaderboard.txt");
        if (file.is_open()) {
            for (int i = 0; i < 5; i++) {
                file << top_scores[i] << std::endl;
            }
            file.close();
        }
    }
}

// --- END OF GameController.cpp ---


// ==========================================
// --- START OF main.cpp ---
// ==========================================

#include "splashkit.h"
// Removed local include: #include "entity.h"
// Removed local include: #include "GameController.h"


int main()
{
    open_window("Game", 1280, 720);

    load_bitmap("Player", "combined_sheet.png");
    load_bitmap("Hellhound", "hellhound.png");
    load_bitmap("EnemyShotSprite", "fireball.png");
    load_bitmap("Boss", "boss.png");
    load_bitmap("FireWall", "fire.png");
    bitmap_set_cell_details(bitmap_named("FireWall"), 32, 48, 8, 1, 8);
    bitmap_set_cell_details(bitmap_named("Player"), 125, 125, 39, 1, 39);
    bitmap_set_cell_details(bitmap_named("Hellhound"), 80, 60, 23, 1, 23);
    bitmap_set_cell_details(bitmap_named("EnemyShotSprite"), 16, 16, 3, 1, 3);
    bitmap_set_cell_details(bitmap_named("Boss"), 160, 120, 37, 1, 37);
    load_animation_script("FireAnim", "fire_anim.txt");
    load_animation_script("BossAnim", "boss_anim.txt");
    load_animation_script("PlayerAnim", "player_anim.txt");
    load_animation_script("HellhoundAnim", "hellhound_anim.txt");
    load_bitmap("ChaserSprite", "chaser.png");
    bitmap_set_cell_details(bitmap_named("ChaserSprite"), 96, 112, 8, 1, 8);
    load_animation_script("ChaserAnim", "chaser_anim.txt");

    GameController game;
 

    timer dt_timer = create_timer("Movement Timer");
    start_timer(dt_timer);
    while (!quit_requested()) {
        process_events();
        clear_screen(COLOR_WHITE);

        double dt = timer_ticks(dt_timer) / 1000.0;
        reset_timer(dt_timer);
        game.run(dt);
        refresh_screen(144);
    }
}
// --- END OF main.cpp ---


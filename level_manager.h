#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include "room.h"
#include "enemy.h"
#include "dynamic_array.h"
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

        reward_type get_random_reward();

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
        void clear_doors();
};

#endif
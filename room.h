#ifndef ROOM_H
#define ROOM_H
#include "dynamic_array.h"
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
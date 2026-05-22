#include "room.h"

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


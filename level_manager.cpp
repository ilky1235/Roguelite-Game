#include "level_manager.h"

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
    else if (roll < 0.66) return DAMAGE;
    else return SPEED;
}

void LevelManager::generate_graph() {
    // Clear old memory if we are restarting
    for (int i = 0; i < all_nodes.length(); i++) {
        delete all_nodes[i];
    }
    all_nodes.clear();
    active_doors.clear();

    root_node = new RoomNode(3, HEAL);
    RoomNode* path_a = new RoomNode(3 + rnd(3), get_random_reward());
    RoomNode* path_b = new RoomNode(3 + rnd(3), get_random_reward());
    RoomNode* boss = new RoomNode(1, BOSS);

    all_nodes.add(root_node);
    all_nodes.add(path_a);
    all_nodes.add(path_b);
    all_nodes.add(boss);

    root_node->add_door(path_a);
    root_node->add_door(path_b);
    path_a->add_door(boss);
    path_b->add_door(boss);

    current_room = root_node;
}

void LevelManager::spawn_wave(dynamic_array<Enemy*> &enemy_arr, int screen_w, int screen_h) {
    if (current_room->get_reward() == BOSS) {
        enemy_arr.add(new BossEnemy(screen_w - 150, screen_h / 2, 0));
        return; 
    }

    int num_enemies = current_room->get_enemies() + 2;

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
        new_door.bounds = rectangle_from(screen_w - 80, start_y + (i * (door_h + spacing)), door_w, door_h);
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
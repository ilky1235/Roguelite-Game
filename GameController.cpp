#include "GameController.h"


GameController::GameController()
{
    for (int i = 0; i < 3; i++) {
        enemy_arr.add(Enemy(100, (i * 200)));
    }
}

void GameController::run(double dt)
{
    if (mouse_clicked(LEFT_BUTTON)) {
        player.attack(mouse_position());
    }
    player.handle_input(dt);
    for (int i = enemy_arr.length() - 1; i >= 0; i--) {
        if (player.get_is_swinging()) {
            point_2d sword_center = player.get_attack_hitbox();
            if (point_point_distance(enemy_arr[i].get_coordinates(), sword_center) <= 65) {
                enemy_arr.remove(i);
                continue;
            }
        }
        enemy_arr[i].update(player, dt, projectiles);
        enemy_arr[i].draw();
    }


    for (int i = projectiles.length() - 1 ; i >= 0; i--) {
        projectiles[i].update(dt);
        projectiles[i].draw();
    }

    player.draw();
    player.draw_hitbox();

}
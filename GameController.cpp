#include "GameController.h"


GameController::GameController()
{
    for (int i = 0; i < 3; i++) {
        enemy_arr.add(Enemy(100, (i * 200)));
    }
}

void GameController::run(double dt)
{
    player.handle_input(dt);
    for (int i = 0; i < enemy_arr.length(); i++) {
        enemy_arr[i].update(player, dt);
        enemy_arr[i].draw();
    }


    for (int i = 0; i < projectiles.length(); i++) {
        projectiles[i].update(dt);
        projectiles[i].draw();
    }

    player.draw();

}
#include "splashkit.h"
#include "entity.h"
#include "GameController.h"


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
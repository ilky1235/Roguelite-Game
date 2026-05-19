#include "splashkit.h"
#include "entity.h"
#include "GameController.h"


int main()
{
    GameController game;
    open_window("Game", 1280, 720);

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
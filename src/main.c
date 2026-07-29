#include "engine/bitboard.h"
#include "engine/chess.h"
#include "ui/window.h"

int main() {
    if (window_init(1280, 720) < 0) return 1;

    GameState game_state = engine_new_game();

    while (!window_should_close()) {
        window_begin_frame();
        window_draw(&game_state);
        window_end_frame();
    }

    window_shutdown();
    return 0;
}
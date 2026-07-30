#include "engine/bitboard.h"
#include "engine/chess.h"
#include "ui/window.h"
#include <string.h>

typedef struct {
    Side your_side;
    int host;
    char *name;
    char *opponent_name;
} ClientState;

int main(int argc, const char *argv[]) {
    ClientState client_state = {0};
    
    if (argc) {
        client_state.host = strcmp(argv[0], "host");
    } // to be replaced with ui input later

    client_state.name = "Rico";
    
    if (window_init(1280, 720) < 0) return 1;

    GameState game_state = engine_new_game();

    while (!window_should_close()) {
        // Poll client connection
        window_begin_frame();
        window_draw(&game_state, client_state.name);
        window_end_frame();
    }

    window_shutdown();
    return 0;
}
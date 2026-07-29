#include "ui/window.h"

int main() {
    if (window_init(1280, 720) < 0) return 1;

    while (!window_should_close()) {
        window_begin_frame();

        
        
        window_end_frame();
    }

    window_shutdown();
    return 0;
}
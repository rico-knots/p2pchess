#include "./event_bus.h"
#include <stdio.h>

#define MAX_LISTENERS 64

typedef struct {
    EventType event_type;
    EventCallback callback;
    void *user_data;
} Listener;

static Listener listeners[MAX_LISTENERS];
static unsigned int n = 0;

void event_bus_subscribe(EventType type, EventCallback callback, void *user_data) {
    if (n == MAX_LISTENERS) {
        printf("[EventBus Error] Listeners array full!\n");
        return;
    }

    listeners[n] = (Listener){type, callback, user_data};
    n++;
}

void event_bus_unsubscribe(EventType type, EventCallback callback, void *user_data) {
    for (unsigned int i = 0; i < n; i++) {
        Listener l = listeners[i];
        if (l.event_type == type && l.callback == callback && l.user_data == user_data) {
            listeners[i] = listeners[n - 1];
            n--;
            return;
        }
    }
}

void event_bus_post(const Event *event) {
    if (!event) return;
    for (unsigned int i = 0; i < n; i++) {
        Listener l = listeners[i];

        if (l.event_type == event->header.type)
            l.callback(event, l.user_data);
    }
}
#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "./../networking/packets.h"

typedef Packet Event;
typedef PacketType EventType;

typedef void (*EventCallback)(const Event *event, void *user_data);

void event_bus_subscribe(EventType type, EventCallback callback, void *user_data);
void event_bus_unsubscribe(EventType type, EventCallback callback, void *user_data);
void event_bus_post(const Event *event);

#endif
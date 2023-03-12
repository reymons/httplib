#ifndef HL_EVENT_H
#define HL_EVENT_H

#include <stdint.h>

#define HL_MAX_EVENTS 255
#define HL_MAX_EVENT_HANDLERS 255
#define HL_EMPTY_HANDLER 0

#define HL_EV_SERV_RUN 1 << 0
#define HL_EV_CONN 1 << 1

#define HL_IS_EV_APPLIED(type, events) (events & type)

typedef void hl_event_handler(void*);
typedef uint8_t hl_event_t;
typedef uint8_t hl_eid_t;

struct hl_event {
    hl_event_t type;
    hl_event_handler **handlers;
    uint8_t max_handler_id;
};

hl_eid_t hl_event_on(const hl_event_t type, hl_event_handler *handler);

void hl_event_off(const hl_event_t type, const hl_eid_t eid);

void hl_event_emit(const hl_event_t type, void *data);

void hl_event_init();

void hl_event_free();

#endif

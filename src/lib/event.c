#include <string.h>
#include "event.h"
#include "logger.h"

static struct hl_event **events;
static struct hl_event* ev;
static int i = 0;
static int max_event = 0;
static int applied_events = 0;

void hl_event_init()
{
    events = malloc(HL_MAX_EVENTS * sizeof(void*));
}

void hl_event_free()
{
    free(events);
}

hl_eid_t hl_event_on(const hl_event_t type, hl_event_handler *handler)
{
    if ((applied_events & type) == 0) {
        applied_events |= type;
        struct hl_event *ev = malloc(sizeof(struct hl_event));
        ev->type = type;
        ev->max_handler_id = 0;
        ev->handlers = malloc(HL_MAX_EVENT_HANDLERS * sizeof(void*));
        memset(ev->handlers, HL_EMPTY_HANDLER, HL_MAX_EVENT_HANDLERS);
        ev->handlers[0] = handler;
        events[max_event] = ev;
        max_event++;
        return 0;
    }

    for (i = 0; i < HL_MAX_EVENTS; i++) {
        ev = events[i];
        if (ev->type != type)
            continue;
        for (i = 0; i < HL_MAX_EVENT_HANDLERS; i++)
            if (ev->handlers[i] == HL_EMPTY_HANDLER) {
                ev->handlers[i] = handler;
                ev->max_handler_id = i;
                return i;
            }
        hl_error("MAX_EVENTS_REACH");
    }

    return -1;
}

void hl_event_off(const hl_event_t type, const hl_eid_t eid)
{
    if (applied_events & type)
        for (i = 0; i < HL_MAX_EVENTS; i++) {
            ev = events[i];
            if (ev->type == type)
                ev->handlers[eid] = NULL;
        }
}

void hl_event_emit(const hl_event_t type, void *data)
{
    if ((applied_events & type) == 0)
        return;

    for (i = 0; i < HL_MAX_EVENTS; i++) {
        ev = events[i];
        if (ev->type == type) {
            for (i = 0; i <= ev->max_handler_id; i++) {
                if (ev->handlers[i] != NULL)
                    ev->handlers[i](data);
            }
            break;
        }
    }
}
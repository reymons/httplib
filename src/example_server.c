#define HL_CONN_BUF_SIZE 8192
#define HL_BACKLOG 256

#include "lib/event.h"
#include "lib/server.h"

void handle_serv_run(void*);
void handle_conn(void*);

int main(int argc, char **argv)
{
    struct hl_serv_opts opts;
    bzero(&opts, sizeof(opts));

    opts.host = getenv("HOST");
    opts.port = atoi(getenv("PORT"));

    struct hl_server *serv = hl_server_create(&opts);

    hl_event_on(HL_EV_SERV_RUN, handle_serv_run);
    hl_event_on(HL_EV_CONN, handle_conn);
    
    hl_server_run(serv);

    return 0;
}

void handle_serv_run(void* data)
{
    struct hl_server *serv = (struct hl_server*)data;
    printf("Server is running on port %i...\n", ntohs(serv->addr.sin_port));
}

void handle_conn(void* data)
{
    struct hl_conn *conn = (struct hl_conn*)data;
    printf("New connection: %i\n", conn->fd);
}

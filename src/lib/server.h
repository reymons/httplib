#ifndef HL_SERVER_H
#define HL_SERVER_H

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "logger.h"
#include "event.h"

#ifndef HL_CONN_BUF_SIZE
#define HL_CONN_BUF_SIZE 4096
#endif

#ifndef HL_FAMILY
#define HL_FAMILY AF_INET
#endif

#ifndef HL_BACKLOG
#define HL_BACKLOG 256
#endif

typedef struct sockaddr hl_sa_t;

struct hl_conn {
    int fd;
    char *buf;
    uint16_t buf_size;
    uint16_t nread;
    uint16_t nwritten;
};

struct hl_serv_opts {
    char *host;
    int port;
    sa_family_t family;
};

struct hl_server {
    int fd;
    struct sockaddr_in addr;
};

struct hl_server* hl_server_create(struct hl_serv_opts *opts);

void hl_server_run(struct hl_server *serv);

void hl_fd_nonblock(const int);

struct hl_conn* hl_malloc_conn(const int fd);

void hl_free_conn(struct hl_conn *conn);

#endif

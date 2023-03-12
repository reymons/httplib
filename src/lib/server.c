#include "server.h"

struct hl_server* hl_server_create(struct hl_serv_opts *opts)
{
    hl_event_init();

    struct hl_server *serv = malloc(sizeof(struct hl_server));

    serv->fd = socket(HL_FAMILY, SOCK_STREAM, 0);

    if (serv->fd < 0)
        hl_error("SOCKET_CREATE");
    
    bzero(&serv->addr, sizeof(serv->addr));
    serv->addr.sin_family = HL_FAMILY;
    serv->addr.sin_port = htons(opts->port);
    inet_pton(HL_FAMILY, opts->host, &serv->addr.sin_addr);

    return serv;
}

void hl_server_run(struct hl_server *serv)
{
    if (bind(serv->fd, (hl_sa_t*)&serv->addr, sizeof(serv->addr)) < 0)
        hl_error("SOCKET_BIND");

    if (listen(serv->fd, HL_BACKLOG) < 0)
        hl_error("SOCKET_LISTEN");

    hl_event_emit(HL_EV_SERV_RUN, serv);

    fd_set rset, wset, allrset, allwset;
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_ZERO(&allrset);
    FD_ZERO(&allwset);
    FD_SET(serv->fd, &allrset);

    int maxfd = serv->fd;
    int connfd, i, maxi, nread, nwritten;
    struct hl_conn *conn;

    struct hl_conn **conns = malloc(HL_BACKLOG * sizeof(void*));

    for (int i = 0; i < HL_BACKLOG; i++)
        conns[i] = NULL;

    while (1) {
        rset = allrset;
        wset = allwset;

        select(maxfd + 1, &rset, &wset, NULL, NULL);

        if (FD_ISSET(serv->fd, &rset)) {
            connfd = accept(serv->fd, NULL, NULL);
            hl_fd_nonblock(connfd);
            conn = hl_malloc_conn(connfd);
            hl_event_emit(HL_EV_CONN, conn);
            for (i = 0; i < HL_BACKLOG; i++) {
                if (conns[i] == NULL) {
                    FD_SET(conn->fd, &allrset);
                    FD_SET(conn->fd, &allwset);
                    conns[i] = conn;
                    break;
                }
            }
            if (i == HL_BACKLOG)
                hl_error("Too many connections\n");
            if (conn->fd > maxfd)
                maxfd = conn->fd;
            if (i > maxi)
                maxi = i;
        }
        
        for (i = 0; i <= maxi; i++) {
            if (conns[i] == NULL)
                continue;

            conn = conns[i];

            if (FD_ISSET(conn->fd, &rset)) {
                nread = read(conn->fd,
                    conn->buf + conn->nread,
                    conn->buf_size - conn->nread
                );
                if (nread < 0) {
                    if (errno == EWOULDBLOCK)
                        continue;
                    FD_CLR(conn->fd, &allrset);
                    hl_free_conn(conn);
                    conns[i] = NULL;
                    continue;
                }
                if (nread == 0) {
                    FD_CLR(conn->fd, &allrset);
                    if (conn->nread == 0) {
                        printf("%i closed abnormally\n", conn->fd);
                        hl_free_conn(conn);
                        conns[i] = NULL;
                        continue;
                    }
                } else {
                    conn->nread += nread;
                    FD_SET(conn->fd, &allwset);
                    FD_SET(conn->fd, &wset);
                }
            }
        }
    }

    hl_event_free();
    free(conns);
    close(serv->fd);
}

void hl_fd_nonblock(const int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
        hl_error("FCNTL_GET");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        hl_error("FCNTL_SET");
}

struct hl_conn* hl_malloc_conn(int fd)
{
    struct hl_conn* conn = malloc(sizeof(struct hl_conn));
    conn->fd = fd;
    conn->buf_size = HL_CONN_BUF_SIZE;
    conn->buf = malloc(conn->buf_size * sizeof(char));
    conn->nread = 0;
    conn->nwritten = 0;
    return conn;
}

void hl_free_conn(struct hl_conn *conn)
{
    free(conn->buf);
    free(conn);
}

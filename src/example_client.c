#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int clifd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(7000);
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(clifd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    write(clifd, "Hello", 5);

    char buf[2048];
    int n = read(clifd, buf, sizeof(buf) - 1);
    buf[n] = 0;

    fputs(buf, stdout);

    return 0;
}

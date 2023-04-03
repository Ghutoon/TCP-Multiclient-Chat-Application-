#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *send_channel(void *client_socket_fd);
void *receive_channel(void *client_socket_fd);

int status_flag = 1;
int main()
{
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("192.168.4.29/22");

    int ret = connect(client_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (ret < 0)
    {
        printf("Could not connect to server.\n");
        exit(1);
    }

    printf("Connection established.\n");

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, &receive_channel, (void *)&client_socket_fd);
    pthread_create(&thread1, NULL, &send_channel, (void *)&client_socket_fd);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    close(client_socket_fd);
}

void *send_channel(void *client_socket_fd)
{
    while (1)
    {
        int fd = *((int *)client_socket_fd);

        char buffer[1024];
        scanf("%s", buffer);

        int ret = send(fd, buffer, sizeof(buffer), 0);

        if (ret < 0)
        {
            printf("Could not send message.\n");
        }

        if (strcmp(buffer, "bye") == 0)
        {
            exit(0);
        }
    }
}

void *receive_channel(void *client_socket_fd)
{
    int fd = *((int *)client_socket_fd);

    char buffer1[1024];
    int ret;
    while (ret = recv(fd, buffer1, sizeof(buffer1), 0))
    {

        if (ret < 0)
        {
            printf("Could not recieve message.\n");
        }
        else
            printf("%s\n", buffer1);
    }
}

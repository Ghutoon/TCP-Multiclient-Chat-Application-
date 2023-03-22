#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int client_fd_map[10] = {-1};
void *routine(void *client_socket_fd);
int main()
{
    //Creating server side socket.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //Binding server socket to port 8080.
    int ret = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (ret < 0)
    {
        printf("Could not bind socket to server.\n");
        exit(1);
    }

    listen(server_socket, 5);

    printf("Listening .......\n");

    int client_socket;

    pthread_t threads[10];
    int cnt = 0;

    //While there is a connection request from a client do the following/
    while (client_socket = accept(server_socket, NULL, NULL))
    {
        //Mapping the client_socket file descriptors to array indeces to identify the client.
        client_fd_map[cnt] = client_socket;

        printf("%d     %d\n", cnt, client_fd_map[cnt]);

        //Each thread handles the communication channel between a client and the server.
        pthread_create(&threads[cnt], NULL, &routine, (void *)&client_socket);
        cnt++;
    }

    for (int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(client_socket);
    close(server_socket);
    return 0;
}

//Communication protocol
void *routine(void *client_socket_fd)
{
    int client_sock = *((int *)client_socket_fd);

    char buffer[1024];
    while (recv(client_sock, buffer, sizeof(buffer), 0))
    {
        if (strcmp(buffer, "bye") == 0)
        {
            printf("Client disconnected.\n");
            pthread_exit(NULL);
        }
        printf("%s\n", buffer);

        int destination_socket_fd = client_fd_map[buffer[0] - 48];

        if(destination_socket_fd == -1)
        {
            char message[256] = "Receiver not found.";
            send(client_sock, message, sizeof(message), 0);
            continue;
        }
        send(destination_socket_fd, buffer, sizeof(buffer), 0);
    }

}
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <poll.h>
#include <unistd.h>

typedef struct pollfd pollfd;

#define STDOUT 1
#define BUFFER_SIZE 1024

const char* SERVER_LAUNCHED = "Server Launched!\n";
const char* NEW_CONNECTION = "New Connection!\n";

int main(int argc, char* argv[])
{
    if(argc != 3)
        perror("Invalid Arguments");

    char* ipaddr = argv[1];
    struct sockaddr_in server_addr;
    int server_fd, opt = 1;
    std::vector<pollfd> pfds;
    
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, ipaddr, &(server_addr.sin_addr)) == -1)
        perror("FAILED: Input ipv4 address invalid");

    if((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        perror("FAILED: Socket was not created");

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable failed");

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable port failed");

    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
    
    server_addr.sin_port = htons(strtol(argv[2], NULL, 10));

    if(bind(server_fd, (const struct sockaddr*)(&server_addr), sizeof(server_addr)) == -1)
        perror("FAILED: Bind unsuccessfull");

    if(listen(server_fd, 20) == -1)
        perror("FAILED: Listen unsuccessfull");

    write(1, SERVER_LAUNCHED, strlen(SERVER_LAUNCHED));

    pfds.push_back(pollfd{server_fd, POLLIN, 0});

    while(1)
    {
        if(poll(pfds.data(), (nfds_t)(pfds.size()), -1) == -1)
            perror("FAILED: Poll");
        
        for(size_t i = 0; i < pfds.size(); ++i)
        {
            if(pfds[i].revents & POLLIN)
            {
                if(pfds[i].fd == server_fd) // new user
                {
                    struct sockaddr_in new_addr;
                    socklen_t new_size = sizeof(new_addr);
                    int new_fd = accept(server_fd, (struct sockaddr*)(&new_addr), &new_size);
                    write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
                    pfds.push_back(pollfd{new_fd, POLLIN, 0});
                }
                else // message from user
                {
                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, BUFFER_SIZE);
                    recv(pfds[i].fd, buffer, BUFFER_SIZE, 0);
                    write(1, buffer, strlen(buffer));
                }
            }
        }
    }
}
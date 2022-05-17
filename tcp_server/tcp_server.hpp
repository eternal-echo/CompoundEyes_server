#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <list>

class TCP_server
{
public:
    TCP_server();
    TCP_server(const char *ip, int port, int backlog):port_(port), backlog_(backlog)
    {
        strcpy(ip_addr_, ip);
        sockfd_ = -1;
    }
    ~TCP_server();
    int init(){init(ip_addr_, port_, backlog_);}
    int init(const char *ip, int port = 8000, int backlog = 5);
    int deinit();
    int accept(int &client_fd, struct sockaddr_in &client_addr);
    int send(int client_fd, const unsigned char *buf, size_t len);
    int send(int client_fd, const char *buf, size_t len);
    int send(int client_fd, const std::string &buf);
    size_t recv(int client_fd, unsigned char *buf, size_t len);
    size_t recv(int client_fd, char *buf, size_t len);
    template <typename P>
    size_t recv(int client_fd, P buf, size_t len)
    {
        return recv(client_fd, (unsigned char *)buf, len);
    }
    size_t recv(int client_fd, std::string &buf);
    size_t recv(int client_fd, std::vector<unsigned char> &buf);
    int close(int client_fd);
    int clear_recv_buffer(int client_fd);
private:
    struct sockaddr_in server_addr;
    int sockfd_;
    char ip_addr_[16];
    int port_;
    int backlog_;
    unsigned char *buf_ = nullptr;
    size_t buf_size_ = 120000;
};

#endif // __TCP_SERVER_HPP__
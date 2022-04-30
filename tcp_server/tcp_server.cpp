#include "tcp_server.hpp"

TCP_server::TCP_server()
{
    sockfd_ = -1;
}

TCP_server::~TCP_server()
{
    deinit();
}

int TCP_server::init(const char *ip, int port, int backlog)
{
    int ret;
    buf_ = new unsigned char[buf_size_];
    struct sockaddr_in server_addr;

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_ < 0) {
        std::cout<< "Failed to create socket" << std::endl;
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    strcpy(ip_addr_, ip);
    port_ = port;
    backlog_ = backlog;

    ret = bind(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0) {
        std::cout<< "Failed to bind socket" << std::endl;
        return -1;
    }

    ret = ::listen(sockfd_, backlog);
    if(ret < 0) {
        std::cout<< "Failed to listen socket" << std::endl;
        return -1;
    }

    return 0;
}

int TCP_server::deinit()
{
    if(sockfd_ >= 0) {
        close(sockfd_);
        sockfd_ = -1;
    }
    if(buf_) {
        delete[] buf_;
        buf_ = NULL;
    }
    return 0;
}

int TCP_server::accept(int &client_fd, struct sockaddr_in &client_addr)
{
    socklen_t client_len = sizeof(client_addr);
    client_fd = ::accept(sockfd_, (struct sockaddr *)&client_addr, &client_len);
    if(client_fd < 0) {
        std::cout<< "Failed to accept connection" << std::endl;
        return -1;
    }
    return 0;
}

int TCP_server::send(int client_fd, const unsigned char* buf, size_t len)
{
    int ret;
    ret = ::send(client_fd, buf, len, 0);
    if(ret < 0) {
        std::cout << "Failed to send data" << std::endl;
        return -1;
    }
    return ret;
}

int TCP_server::send(int client_fd, const char* buf, size_t len)
{
    int ret;
    ret = ::send(client_fd, buf, len, 0);
    if(ret < 0) {
        std::cout << "Failed to send data" << std::endl;
        return -1;
    }
    return ret;
}

int TCP_server::send(int client_fd, const std::string& buf)
{
    return send(client_fd, (unsigned char*)buf.c_str(), buf.size());
}

size_t TCP_server::recv(int client_fd, unsigned char* buf, size_t len)
{
    size_t ret;
    ret = ::recv(client_fd, buf, len, 0);
    if(ret < 0) {
        std::cout << "Failed to recv data" << std::endl;
        return -1;
    }
    return 0;
}

size_t TCP_server::recv(int client_fd, char* buf, size_t len)
{
    size_t ret;
    ret = ::recv(client_fd, buf, len, 0);
    if(ret < 0) {
        std::cout << "Failed to recv data" << std::endl;
        return -1;
    }
    return ret;
}

size_t TCP_server::recv(int client_fd, std::string& buf)
{
    size_t ret;
    ret = ::recv(client_fd, buf_, buf_size_, 0);
    if(ret < 0) {
        std::cout << "Failed to recv data" << std::endl;
        return -1;
    }
    buf = std::string((char*)buf_, ret);
    return buf.size();
}

size_t TCP_server::recv(int client_fd, std::vector<unsigned char>& buf)
{
    size_t ret;
    buf.clear();
    ret = ::recv(client_fd, buf_, buf_size_, 0);
    if(ret < 0) {
        std::cout << "Failed to recv data" << std::endl;
        return -1;
    }
    buf.assign(buf_, buf_ + ret);
    std::cout << "recv data size: " << ret << " " << buf.size() << std::endl;
    return ret;
}
int TCP_server::close(int client_fd)
{
    int ret;
    ret = ::close(client_fd);
    if(ret < 0) {
        std::cout << "Failed to close connection" << std::endl;
        return -1;
    }
    return 0;
}

int TCP_server::clear_recv_buffer(int client_fd)
{
    char buf[1024];
    int ret;
    do {
        ret = ::recv(client_fd, buf, sizeof(buf), MSG_DONTWAIT);
    } while(ret > 0);
    return 0;
}
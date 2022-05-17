#include "compound_eyes.hpp"

CompoundEyes::CompoundEyes()
{

}

CompoundEyes::~CompoundEyes()
{
    deinit();
}

int CompoundEyes::init()
{
    int ret;
    ret = server_.init("192.168.43.169", 8000, 5);
    if(ret < 0) {
        std::cout<< "Failed to init server" << std::endl;
        return -1;
    }
    // start listening all the time
    listen_thread_ = std::thread(&CompoundEyes::listen, this);
    listen_thread_.detach();
    return 0;
}

int CompoundEyes::deinit()
{
    // TODO: stop listen thread
    server_.deinit();
    return 0;
}

int CompoundEyes::listen()
{
    int ret;
    while(1)
    {
        int client_fd;
        struct sockaddr_in client_addr;
        ret = server_.accept(client_fd, client_addr);
        if(ret < 0) {
            std::cout<< "Failed to accept client" << std::endl;
            continue;
        }
        std::cout<< "Accepted client" << std::endl;
        std::cout<< "IP: " << inet_ntoa(client_addr.sin_addr) << std::endl;
        std::cout<< "Port: " << ntohs(client_addr.sin_port) << std::endl;
        std::cout<< "Client fd: " << client_fd << std::endl;
        Ommatidia_server *o = new Ommatidia_server(server_, ommatidia_servers_.size(), client_fd);
        ret = o->init();
        if(ret < 0) {
            std::cout<< "Failed to init ommatidia server" << std::endl;
            continue;
        }
        std::cout<< "Cameras num: " << o->get_cameras_num() << std::endl;
        ommatidia_servers_.push_back(o);
        ommatidia_threads_.push_back(std::thread(&Ommatidia_server::run, o));
        ommatidia_threads_.back().detach();
    }
    return 0;
}

int CompoundEyes::run()
{
    int ret;
    char cmd;
    std::vector<std::future<int>> futures;
    while(1)
    {
        std::cout << "Waiting for command: " << std::endl;
        std::cin >> cmd;
        switch(cmd) {
            case 'p':
                for(auto &o : ommatidia_servers_) {
                    o->ctrl(cmd);
                }
                break;
            case 'c':
                futures.clear();
                for(auto &o : ommatidia_servers_) {
                    futures.push_back(std::async(std::launch::async, &Ommatidia_server::ctrl, o, std::ref(cmd)));
                }
                for(auto &f : futures) {
                    f.get();
                }
                break;
            case 's':
                for(auto &o : ommatidia_servers_) {
                    o->ctrl(cmd);
                }
                break;
            default:
                std::cout << "Invalid command" << std::endl;
                break;
        }
        cmd = '\0';
    }
    return 0;
}
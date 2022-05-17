#ifndef __OMMATIDIA_SERVER_HPP__
#define __OMMATIDIA_SERVER_HPP__

#include "../tcp_server/tcp_server.hpp"
#include "../JPEG/jpeg_data.hpp"
#include <iostream>
#include <string>
#include <sstream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <opencv2/opencv.hpp>

class Ommatidia_server
{
public:
    Ommatidia_server(TCP_server &server, int camera_id, int fd);
    ~Ommatidia_server();
    int init();
    int deinit();
    int run();
    int ctrl(char &cmd);
    int get_cameras_num();
private:
    int preview();
    int stop_preview();
    int capture();
    int thread_preview();
    int (Ommatidia_server::*state_)() = nullptr;
    TCP_server &server_;
    int client_fd_;
    int cameras_num_;
    int id_;
    bool is_preview_ = false;
    std::vector<unsigned char> v_buffer_ = {};
    std::mutex camera_mutex_;
    std::condition_variable camera_cond_;
    std::thread preview_thread_;
};
#endif
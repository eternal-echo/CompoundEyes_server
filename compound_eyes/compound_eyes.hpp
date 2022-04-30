#ifndef __COMPOUND_EYES_HPP__
#define __COMPOUND_EYES_HPP__
#include "../tcp_server/tcp_server.hpp"
#include "../ommatidia_server/ommatidia_server.hpp"

#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include <future>
#include <mutex>
#include <opencv2/opencv.hpp>

class CompoundEyes
{
public:
    CompoundEyes();
    ~CompoundEyes();
    int init();
    int deinit();
    int run();
private:
    int listen();

    TCP_server server_ = {};
    std::thread listen_thread_;
    std::vector<std::thread> ommatidia_threads_ = {};
    std::vector<Ommatidia_server* > ommatidia_servers_ = {};
};
#endif
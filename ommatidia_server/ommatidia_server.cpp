#include "ommatidia_server.hpp"

Ommatidia_server::Ommatidia_server(TCP_server &server, int camera_id, int fd)
    : server_(server), client_fd_(fd), id_(camera_id)
{
}

Ommatidia_server::~Ommatidia_server()
{
    deinit();
}

int Ommatidia_server::init()
{
    int ret;
    unsigned char cameras_num = 0;
    ret = server_.recv(client_fd_, &cameras_num, 1);
    if(ret < 0) {
        std::cout<< "Failed to recv cameras num" << std::endl;
        return -1;
    }
    std::cout<< "Cameras num: " << (int)cameras_num << std::endl;
    cameras_num_ = cameras_num; 

    preview_thread_ = std::thread(&Ommatidia_server::thread_preview, this);
    preview_thread_.detach();
    return 0;
}

int Ommatidia_server::deinit()
{
    
}

int Ommatidia_server::run()
{
    int ret;
    while(1) {
        if(state_!=nullptr) {
            ret = (this->*state_)();
            if(ret < 0) {
                std::cout<< "Failed to run ommatidia, try again" << std::endl;
            }
        }
    }
    return 0;
}

int Ommatidia_server::ctrl(char &cmd)
{
    int ret;
    switch (cmd)
    {
        case 'p':
            state_ = &Ommatidia_server::preview;
            break;
        case 'c':
            state_ = &Ommatidia_server::capture;
            while(state_ == &Ommatidia_server::capture);
            break;
        case 's':
            if(is_preview_) {
                state_ = &Ommatidia_server::stop_preview;
            }
            else {
                state_ = nullptr;
            }
            break;
        default:
            break;
    }
    return 0;
}

int Ommatidia_server::preview()
{
    std::cout << "Ommatidia preview" << std::endl;
    std::unique_lock<std::mutex> lock(camera_mutex_);
    is_preview_ = true;
    server_.send(client_fd_, "p", 1);
    camera_cond_.notify_one();
    state_ = nullptr;
    return 0;
}

int Ommatidia_server::stop_preview()
{
    std::unique_lock<std::mutex> lock(camera_mutex_);
    is_preview_ = false;
    server_.send(client_fd_, "s", 1);
    camera_cond_.notify_one();
    state_ = nullptr;
    return 0;
}

int Ommatidia_server::thread_preview()
{
    int ret;
    std::vector<unsigned char> jpeg_data;
    while(1) {
        std::unique_lock<std::mutex> lk(camera_mutex_);
        camera_cond_.wait(lk, [this]{return is_preview_;});
        lk.unlock();
        v_buffer_.clear();
        while(is_preview_)
        {
            ret = server_.recv(client_fd_, v_buffer_);
            if(ret < 0) {
                std::cout<< "Failed to recv data" << std::endl;
                continue;
            }
            JPEG_data jpeg(v_buffer_);
            ret = jpeg.find_data(jpeg_data);
            if(ret < 0) {
                std::cout<< "Failed to find data" << std::endl;
                continue;
            }
            cv::Mat img = cv::imdecode(jpeg_data, CV_LOAD_IMAGE_COLOR);
            std::stringstream ss;
            ss << "preview_" << id_ ;
            cv::imshow(ss.str(), img);
            cv::waitKey(1);
        }
    }
    return 0;
}

int Ommatidia_server::capture()
{
    std::unique_lock<std::mutex> lock(camera_mutex_);
    if(is_preview_) {
        // Firstly,preview thread should be stopped
        is_preview_ = false;
        camera_cond_.notify_one();
        // Then, send stop preview command
        server_.send(client_fd_, "s", 1);
    }
    // Clear buffer
    server_.clear_recv_buffer(client_fd_);
    // send capture command
    server_.send(client_fd_, "c", 1);
    
    std::cout << "Ommatidia capture" << std::endl;
    int ret;
    char acks[2];
    std::vector<unsigned char> chunk;
    do {
        for(int i=0; i<cameras_num_; i++) {
            v_buffer_.clear();
            acks[1] = i;
            try {
                std::cout << "try to recv data, camera index:" << i << std::endl;
                size_t buffer_size = 0;
                ret = server_.recv(client_fd_, &buffer_size, sizeof(size_t));
                if(ret < 0) {
                    throw "Failed to recv buffer size";
                }
                std::cout << "The expected receive buffer size: " << buffer_size << std::endl;
                do {
                    chunk.clear();
                    ret = server_.recv(client_fd_, chunk);
                    if(ret < 0) {
                        break;
                    }
                    v_buffer_.insert(v_buffer_.end(), chunk.begin(), chunk.end());
                }while(v_buffer_.size() < buffer_size);
                if(ret < 0) {
                    throw "Failed to recv data";
                }
                std::cout << "The actual receive buffer size: " << v_buffer_.size() << std::endl;
                std::cout << "check JPEG data" << std::endl;
                JPEG_data jpeg(v_buffer_);
                if(!jpeg.is_valid()) {
                    ret = -1;
                    throw "Failed to find data";
                }
                ret = 0;
                acks[0] = 'y';
                // clear the recv buffer before the acks
                server_.clear_recv_buffer(client_fd_);
                server_.send(client_fd_, acks, 2); // jpeg data is valid
                std::cout << "save data" << std::endl;
                cv::Mat img = cv::imdecode(v_buffer_, CV_LOAD_IMAGE_COLOR);
                cv::imwrite("photo/photo" + std::to_string(id_) + "-" + std::to_string(i) + ".jpg", img);
            } catch(const char *e) {
                std::cout<< "Failed to recv data, in capture mode:" << std::endl;
                std::cout<< e << std::endl;
                server_.clear_recv_buffer(client_fd_);
                // when it is not valid, send 'n' to client
                // and wait for the next data
                // so we must clear the recv buffer before the acks
                acks[0] = 'n';
                server_.send(client_fd_, acks, 2); // jpeg data is not valid
                break;
            }
        }
    }while(ret < 0); // if find data failed, try again
    state_ = nullptr;
    return 0;
}

int Ommatidia_server::get_cameras_num()
{
    return cameras_num_;
}
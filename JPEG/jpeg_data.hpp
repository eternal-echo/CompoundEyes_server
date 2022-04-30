#ifndef __JPEG_DATA_HPP__
#define __JPEG_DATA_HPP__

#include <unistd.h>
#include <vector>

class JPEG_data
{
public:
    JPEG_data(std::vector<unsigned char> &buf);
    ~JPEG_data();

    bool is_valid();
    int find_data(std::vector<unsigned char> &buf);

private:
    std::vector<unsigned char> &data_;
    size_t size_;
};


#endif
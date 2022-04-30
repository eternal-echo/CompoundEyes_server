#include "jpeg_data.hpp"

JPEG_data::JPEG_data(std::vector<unsigned char> &buf):
    data_(buf)
{
    size_ = buf.size();
}

JPEG_data::~JPEG_data()
{
    
}

bool JPEG_data::is_valid()
{
    if(data_[0] == 0xff && data_[1] == 0xd8 && data_[size_ - 2] == 0xff && data_[size_ - 1] == 0xd9)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int JPEG_data::find_data(std::vector<unsigned char> &buf) 
{
    size_t i;
    for(i = 0; i < size_; i++)
    {
        if(data_[i] == 0xff && data_[i + 1] == 0xd8 && data_[i + 2] == 0xff && data_[i + 3] == 0xe0)
        {
            break;
        }
    }
    if(i == size_)
    {
        return -1;
    }
    auto header = i;
    size_t jpeg_size = 0;
    for(i = 0; i < size_; i++)
    {
        if(data_[i] == 0xff && data_[i + 1] == 0xd9)
        {
            break;
        }
    }
    if(i == size_)
    {
        return -1;
    }
    jpeg_size = i - header;
    buf.clear();
    buf.insert(buf.end(), data_.begin() + header, data_.begin() + header + jpeg_size);
    return 0;
}
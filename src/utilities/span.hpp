#pragma once 

template<typename T>
struct span 
{
    span(T* data, size_t size)
        : data_(data)
        , size_(size)
    {}


    T& operator[] (size_t index) const {
        return data_[index];
    }

    T* data_;
    size_t size_;
};
#ifndef BUFFER_H
#define BUFFER_H

#include <SmingCore/SmingCore.h>


template<class type, uint8_t size>
class Buffer {
public:
    Buffer() :
            tail(size - 1) {
    }

    void append(const type& data) {
        this->data[this->tail = (this->tail + 1) % size] = data;
    }

    const type& operator[](const uint8_t& index) const {
        return this->data[(this->tail + 1 + index) % size];
    }

private:
    type data[size];
    uint8_t tail;
};


#endif

#ifndef STORE_H
#define STORE_H

#include <SmingCore/SmingCore.h>


template<typename V>
class Store {
    struct record {
        uint32_t sequence;
        V value;
    };

    static constexpr const uint8_t RECORD_SIZE = sizeof(struct record) / INTERNAL_FLASH_WRITE_UNIT_SIZE * INTERNAL_FLASH_WRITE_UNIT_SIZE;

    static constexpr const uint32_t FLASH_ADDR = 0x80000;
    static constexpr const uint32_t FLASH_SIZE = 0x2000;

    static constexpr const uint8_t MAGIC_SIZE = INTERNAL_FLASH_WRITE_UNIT_SIZE * 2;
    static constexpr const char const MAGIC_VALUE[MAGIC_SIZE] = "1234567890";

public:
    explicit Store() {
        // Try to read the magic value and 'format' if not found
        char magic[MAGIC_SIZE];
        spi_flash_read(FLASH_ADDR, magic, MAGIC_SIZE);
        if (memcmp(magic, MAGIC_VALUE, MAGIC_SIZE) != 0) {
            // Erase the whole flash area and set magic
            spi_flash_erase(FLASH_ADDR, FLASH_SIZE);
            spi_flash_write(FLASH_ADDR, MAGIC_VALUE, MAGIC_SIZE);

            // Set the last address ahead of the start and initialize with default value
            this->address = FLASH_ADDR + MAGIC_SIZE - RECORD_SIZE;
            this->sequence = 0;
            this->value = V{};

        } else {

        }

    }

    ~Store() {
    }

    V read() const {
    }

    void write(const V& value) {
    }

private:
    // The address of the last record
    uint32_t address;

    // The sequence number of the last value
    uint32_t sequence;

    // The last value
    V value;
};


#endif

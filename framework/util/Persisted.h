#ifndef PERSISTED_H
#define PERSISTED_H

#include <SmingCore/SmingCore.h>


/**
 * A simple hash function
 *
 * This implements the SDBM hash algorithm.
 */
constexpr uint32_t hash(const char* const s) {
    return (*s != '\0')
           ? *s + hash(s + 1) * 65599
           : 5381;
}


template<typename V>
class Persisted {
    static const Logger LOG;

    struct address_t {
        uint16_t sector;
        uint16_t offset;

        operator uint32_t() const {
            return this->sector * SPI_FLASH_SEC_SIZE + this->offset;
        }
    };

    using sequence_t = uint8_t;

    static constexpr uint32_t FLASH_ADDR = 0x80000;
    static constexpr uint32_t FLASH_SIZE = 0x2000;

    static_assert(FLASH_ADDR % SPI_FLASH_SEC_SIZE == 0, "Flash address must be aligned by sectors");
    static_assert(FLASH_SIZE % SPI_FLASH_SEC_SIZE == 0, "Flash size must be aligned by sectors");

    struct record_t {
        sequence_t sequence;
        V value;
    };

    /**
     * Align record size to the next multiplier of flash write unit size.
     */
    static constexpr uint16_t RECORD_ALIGN = (((sizeof(record_t) - 1) / INTERNAL_FLASH_WRITE_UNIT_SIZE) + 1) * INTERNAL_FLASH_WRITE_UNIT_SIZE;

    static constexpr uint32_t MAGIC[] = {
            0xC0FEBABE,
            0xDEADBEEF,
//            hash("ESP"),
//            hash(DEVICE),
    };

    static_assert(sizeof(MAGIC) % INTERNAL_FLASH_WRITE_UNIT_SIZE == 0,
                  "Magic identifier size must be multiple of flash write unit size");

public:
    explicit Persisted(const V& defaultValue = V{}) :
            sequence(0) {
        // Read the magic identifier
        uint32_t magic[sizeof(MAGIC)];
        flash_read(FLASH_ADDR, magic, sizeof(MAGIC));

        if (memcmp(magic, MAGIC, sizeof(MAGIC)) != 0) {
            LOG.log("Invalid magic identifier - Formatting");

            // Erase the first sector
            flash_erase_sector(FLASH_ADDR / SPI_FLASH_SEC_SIZE);

            // Write the magic identifier
            flash_write(FLASH_ADDR,
                        (uint32_t*) MAGIC,
                        sizeof(MAGIC));

            // Write the initial record right after the magic identifier
            this->address = {
                    .sector = 0,
                    .offset = sizeof(MAGIC)
            };
            this->sequence = 1;
            this->value = defaultValue;

            record_t record = {
                    .sequence = this->sequence,
                    .value = this->value,
            };

            flash_write(FLASH_ADDR + this->address,
                        (uint32_t*) &record,
                        sizeof(record));

        } else {
            LOG.log("Valid magic identifier found - Searching last record");

            address_t next_address = {
                    .sector = 0,
                    .offset = sizeof(MAGIC)
            };
            sequence_t next_sequence = 1;

            while (true) {
                LOG.log("Loading record from", next_address);

                record_t record;
                flash_read(FLASH_ADDR + next_address,
                           (uint32_t*) &record,
                           sizeof(record));

                LOG.log("Got Record with sequence", record.sequence, "expecting", next_sequence);

                // Found an invalid / not consecutive record - assuming the previous one as last valid one
                if (record.sequence != next_sequence) {
                    break;
                }

                this->address = next_address;
                this->sequence = next_sequence;
                this->value = record.value;

                next_address = incr_address(next_address);
                next_sequence = incr_sequence(next_sequence);
            }

            LOG.log("Last valid record found with sequence", this->sequence, "at", this->address);
        }
    }

    Persisted(const Persisted&) = delete;

    ~Persisted() {
    }

    const V& get() const {
        return this->value;
    }

    const V& operator*() const {
        return this->value;
    }

    void write(const V& value) {
        address_t next_address = incr_address(this->address);
        sequence_t next_sequence = incr_sequence(this->sequence);

        // If we write to an sector for the first time, we have to erase it
        if (next_address.sector != this->address.sector) {
            flash_erase_sector(FLASH_ADDR / SPI_FLASH_SEC_SIZE + next_address.sector);

            // Ensure magic identifier still exists
            if (next_address.sector == 0) {
                flash_write(FLASH_ADDR,
                            (uint32_t*) MAGIC,
                            sizeof(MAGIC));
            }
        }

        record_t record = {
                .sequence = next_sequence,
                .value = value,
        };

        LOG.log("Persisting record", next_sequence, "to", next_address);
        flash_write(FLASH_ADDR + next_address,
                    (uint32_t*) &record,
                    sizeof(record));

        this->address = next_address;
        this->sequence = next_sequence;
        this->value = value;
    }

private:
    /**
     * Finds the next address after the given one.
     */
    static address_t incr_address(const address_t& curr) {
        address_t next;
        next.sector = curr.sector;
        next.offset = curr.offset + RECORD_ALIGN;

        // If the next record would protrude into the next sector, align it to the sector start
        if (next.offset + RECORD_ALIGN > SPI_FLASH_SEC_SIZE) {
            next.sector = (next.sector + 1) % (FLASH_SIZE / SPI_FLASH_SEC_SIZE);
            next.offset = next.sector == 0
                          ? sizeof(MAGIC)
                          : 0;
        }

        return next;
    }

    /**
     * Find the next sequence number.
     */
    static sequence_t incr_sequence(const sequence_t curr) {
        return (curr + 1) % 0xFE;
    }

    static void flash_erase_sector(const uint16_t& sect) {
        LOG.log("Flash: Erasing Sector", sect);
        const auto& ret = spi_flash_erase_sector(sect);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Failed!");
        }
    }

    static void flash_write(const uint32_t& addr,
                            uint32_t* data,
                            const uint32_t& size) {
        LOG.log("Flash: Write", addr, size);
        const auto& ret = spi_flash_write(addr,
                                          data,
                                          size);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Failed!");
        }
    }

    static void flash_read(const uint32_t& addr,
                            uint32_t* data,
                            const uint32_t& size) {
        LOG.log("Flash: Read", addr, size);
        const auto& ret = spi_flash_read(addr,
                                         data,
                                         size);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Failed!");
        }
    }

    // The sequence of the last record
    sequence_t sequence;

    // The address of the last record
    address_t address;

    // The last value
    V value;
};


template<typename V>
const Logger Persisted<V>::LOG = Logger("persisted");

template<typename V>
constexpr uint32_t Persisted<V>::MAGIC[];

template<typename V>
constexpr uint32_t Persisted<V>::FLASH_ADDR;

template<typename V>
constexpr uint32_t Persisted<V>::FLASH_SIZE;

#endif

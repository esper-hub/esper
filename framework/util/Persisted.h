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


template<typename V, uint32_t flash_addr = 0x80000, uint32_t flash_size = 0x2000>
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

    static_assert(flash_addr % SPI_FLASH_SEC_SIZE == 0, "Flash address must be aligned by sectors");
    static_assert(flash_size % SPI_FLASH_SEC_SIZE == 0, "Flash size must be aligned by sectors");

    struct record_t {
        sequence_t sequence;
        V value;
    };

    /**
     * Align record size to the next multiplier of flash write unit size.
     */
    static constexpr uint16_t RECORD_ALIGN = (((sizeof(record_t) - 1) / INTERNAL_FLASH_WRITE_UNIT_SIZE) + 1) * INTERNAL_FLASH_WRITE_UNIT_SIZE;

    /**
     * The magic identifier used to verify persistence storage.
     */
    static constexpr uint32_t MAGIC[] = {
            hash("ESP"),
            hash(DEVICE),
    };

    static_assert(sizeof(MAGIC) % INTERNAL_FLASH_WRITE_UNIT_SIZE == 0,
                  "Magic identifier size must be multiple of flash write unit size");

    static_assert(((flash_size - sizeof(MAGIC)) / RECORD_ALIGN) % 0xFE != 0,
                  "Usable space must not fit maximum sequence count of elements");

public:
    explicit Persisted(const V& defaultValue = V{}) :
            sequence(0) {
        // Read the magic identifier
        uint32_t magic[sizeof(MAGIC)];
        flash_read(flash_addr, magic, sizeof(MAGIC));

        if (memcmp(magic, MAGIC, sizeof(MAGIC)) != 0) {
            LOG.log("Invalid magic identifier - Formatting");

            // Erase the first sector
            flash_erase_sector(flash_addr / SPI_FLASH_SEC_SIZE);

            // Write the magic identifier
            flash_write(flash_addr,
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

            flash_write(flash_addr + this->address,
                        (uint32_t*) &record,
                        sizeof(record));

        } else {
            LOG.log("Valid magic identifier found - Searching last record");

            // Start scanning right after magic identifier
            address_t next_address = {
                    .sector = 0,
                    .offset = sizeof(MAGIC)
            };

            // Read the first record - assuming it is always valid
            record_t record;
            flash_read(flash_addr + next_address,
                       (uint32_t*) &record,
                       sizeof(record));

            // Start scanning with the sequence from the first record
            sequence_t next_sequence = record.sequence;

            this->address = next_address;
            this->sequence = next_sequence;
            this->value = record.value;

            // Scan through records until sequence breaks continuity
            while (true) {
                // Load record from next address
                record_t record;
                flash_read(flash_addr + next_address,
                           (uint32_t*) &record,
                           sizeof(record));

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
            flash_erase_sector(flash_addr / SPI_FLASH_SEC_SIZE + next_address.sector);

            // Ensure magic identifier still exists
            if (next_address.sector == 0) {
                flash_write(flash_addr,
                            (uint32_t*) MAGIC,
                            sizeof(MAGIC));
            }
        }

        record_t record = {
                .sequence = next_sequence,
                .value = value,
        };

        LOG.log("Persisting record", next_sequence, "to", next_address);
        flash_write(flash_addr + next_address,
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
            next.sector = (next.sector + 1) % (flash_size / SPI_FLASH_SEC_SIZE);
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
        const auto& ret = spi_flash_erase_sector(sect);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Erase failed:", sect);
        }
    }

    static void flash_write(const uint32_t& addr,
                            uint32_t* data,
                            const uint32_t& size) {
        const auto& ret = spi_flash_write(addr,
                                          data,
                                          size);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Write failed:", addr);
        }
    }

    static void flash_read(const uint32_t& addr,
                            uint32_t* data,
                            const uint32_t& size) {
        const auto& ret = spi_flash_read(addr,
                                         data,
                                         size);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Read failed:", addr);
        }
    }

    // The sequence of the last record
    sequence_t sequence;

    // The address of the last record
    address_t address;

    // The last value
    V value;
};


template<typename V, uint32_t flash_addr, uint32_t flash_size>
const Logger Persisted<V, flash_addr, flash_size>::LOG = Logger("persisted");

template<typename V, uint32_t flash_addr, uint32_t flash_size>
constexpr uint32_t Persisted<V, flash_addr, flash_size>::MAGIC[];

#endif

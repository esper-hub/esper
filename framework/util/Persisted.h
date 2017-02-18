#ifndef PERSISTED_H
#define PERSISTED_H

#include <SmingCore/SmingCore.h>


template<typename value_t, uint32_t flash_addr = 0x80000, uint32_t flash_size = 0x2000>
class Persisted {
    static const Logger LOG;

    struct address_t {
        uint16_t sector;
        uint16_t offset;
    };

    using sequence_t = uint8_t;
    using checksum_t = uint16_t;

    static_assert(flash_addr % SPI_FLASH_SEC_SIZE == 0, "Flash address must be aligned by sectors");
    static_assert(flash_size % SPI_FLASH_SEC_SIZE == 0, "Flash size must be aligned by sectors");

    struct record_t {
        sequence_t sequence;
        value_t value;

        checksum_t checksum;
    };

    /**
     * Align record size to the next multiplier of flash write unit size.
     */
    static constexpr uint16_t RECORD_ALIGN = (((sizeof(record_t) - 1) / INTERNAL_FLASH_WRITE_UNIT_SIZE) + 1) * INTERNAL_FLASH_WRITE_UNIT_SIZE;

    static_assert((flash_size / RECORD_ALIGN) % 0xFE != 0,
                  "Usable space must not fit maximum sequence count of elements");

public:
    explicit Persisted(const value_t& defaultValue = value_t{}) :
            sequence(0) {

        // Start scanning right after magic identifier
        address_t next_address = {
                .sector = 0,
                .offset = 0,
        };

        // Read the first record
        record_t record;
        flash_read(next_address,
                   record);

        // If first record is invalid, reinitialize the storage
        if (record.checksum != checksum(record)) {
            LOG.log("Invalid first record - Formatting");

            // Erase the first sector
            flash_erase_sector(next_address.sector);

            // Write the initial record
            this->address = next_address;
            this->sequence = 1;
            this->value = defaultValue;

            record_t record;
            record.sequence = this->sequence;
            record.value = this->value;
            record.checksum = checksum(record);

            flash_write(this->address,
                        record);

        } else {
            LOG.log("Valid first record - Searching last record");

            // Start scanning with the sequence from the first record
            sequence_t next_sequence = record.sequence;

            this->address = next_address;
            this->sequence = next_sequence;
            this->value = record.value;

            // Scan through records until sequence breaks continuity
            while (true) {
                // Load record from next address
                flash_read(next_address,
                           record);

                // Found an invalid / not consecutive record - assuming the previous one as last valid one
                if (record.sequence != next_sequence || record.checksum != checksum(record)) {
                    break;
                }

                // Remember this record as last valid one
                this->address = next_address;
                this->sequence = next_sequence;
                this->value = record.value;

                // Move to next record
                next_address = incr_address(next_address);
                next_sequence = incr_sequence(next_sequence);
            }

            LOG.log("Last valid record found with sequence", this->sequence, "at", this->address.sector, "@", this->address.offset);
        }
    }

    Persisted(const Persisted&) = delete;

    ~Persisted() {
    }

    const value_t& get() const {
        return this->value;
    }

    const value_t& operator*() const {
        return this->value;
    }

    void write(const value_t& value) {
        address_t next_address = incr_address(this->address);
        sequence_t next_sequence = incr_sequence(this->sequence);

        // If we write to an sector for the first time, we have to erase it
        if (next_address.sector != this->address.sector) {
            flash_erase_sector(next_address.sector);
        }

        record_t record;
        record.sequence = next_sequence;
        record.value = value;
        record.checksum = checksum(record);

        LOG.log("Persisting record", next_sequence, "to", next_address.sector, "@", next_address.offset);
        flash_write(next_address,
                    record);

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
            next.offset = 0;
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
        const auto& ret = spi_flash_erase_sector(flash_addr / SPI_FLASH_SEC_SIZE + sect);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Erase failed:", sect);
        }
    }

    static void flash_write(const address_t& addr,
                            const record_t& record) {
        const auto& ret = spi_flash_write(flash_addr + addr.sector * SPI_FLASH_SEC_SIZE + addr.offset,
                                          (uint32_t*) &record,
                                          sizeof(record_t));
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Write failed:", addr.sector, "@", addr.offset);
        }
    }

    static void flash_read(const address_t& addr,
                           record_t& record) {
        const auto& ret = spi_flash_read(flash_addr + addr.sector * SPI_FLASH_SEC_SIZE + addr.offset,
                                         (uint32_t*) &record,
                                         sizeof(record_t));
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Read failed:", addr.sector, "@", addr.offset);
        }
    }

    /**
     * Implementation of SDBM hash algorithm.
     */
    static constexpr uint16_t hash(const char* const s) {
        return (*s != '\0') ? *s + hash(s + 1) * 65599 : 5381;
    }

    /**
     * Implementation of CRC-16-CCITT algorithm.
     */
    static uint16_t checksum(const record_t& record) {
        // Use the hash of the device name as start value to distinguish between different device types
        unsigned short crc = hash(DEVICE);

        // Loop over the records bytes but skip the checksum
        for (uint16_t i = 0; i < sizeof(record_t) - sizeof(uint16_t); i++) {
            unsigned char x = crc >> 8;
            x ^= ((char*) &record)[i];
            x ^= x >> 4;

            crc = (crc << 8);
            crc ^= (x << 12);
            crc ^= (x << 5);
            crc ^= (x << 0);
        }

        return crc;
    }

    // The sequence of the last record
    sequence_t sequence;

    // The address of the last record
    address_t address;

    // The last value
    value_t value;
};


template<typename value_t, uint32_t flash_addr, uint32_t flash_size>
const Logger Persisted<value_t, flash_addr, flash_size>::LOG = Logger("persisted");

#endif

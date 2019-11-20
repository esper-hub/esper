#ifndef PERSISTED_H
#define PERSISTED_H

#include <SmingCore.h>

#include "Checksum.h"


template<typename value_t, uint32_t flash_addr = 0x80000, uint32_t flash_size = 0x2000>
class Persisted {
    static_assert(flash_addr % SPI_FLASH_SEC_SIZE == 0,
                  "Flash address must be aligned by sectors");

    static_assert(flash_size % SPI_FLASH_SEC_SIZE == 0,
                  "Flash size must be aligned by sectors");

    static const Logger LOG;

    using sequence_t = uint32_t;

    struct record_t {
        sequence_t sequence;
        value_t value;

        Checksum::value_t checksum;

        friend Checksum operator<<(Checksum checksum, const record_t& record) {
            return checksum
                   << record.sequence
                   << record.value;
        }
    } __attribute__((aligned(4)));

    struct address_t {
        uint16_t sector;
        uint16_t offset;
    };

public:
    explicit Persisted(const value_t& defaultValue = value_t{}) {
        // Start scanning right after magic identifier
        address_t address;
        address.sector = 0;
        address.offset = 0;

        // Read the first record
        record_t record;
        flash_read(address,
                   record);

        // If first record is invalid, reinitialize the storage
        if (record.checksum != checksum(record)) {
            LOG.log(F("Invalid first record - Formatting"));

            // Erase the first sector
            flash_erase_sector(address.sector);

            // Write the initial record
            record.sequence = this->sequence = 1;
            record.value = this->value = defaultValue;
            record.checksum = checksum(record);

            flash_write(this->address = address,
                        record);

        } else {
            LOG.log(F("Valid first record - Searching last record"));

            // Start scanning with the sequence from the first record
            sequence_t sequence = record.sequence;

            this->address = address;
            this->sequence = sequence;
            this->value = record.value;

            // Scan through records until sequence breaks continuity
            while (true) {
                // Load record from next address
                flash_read(address,
                           record);

                // Found an invalid / not consecutive record - assuming the previous one as last valid one
                if (record.sequence != sequence || record.checksum != checksum(record)) {
                    break;
                }

                // Remember this record as last valid one
                this->address = address;
                this->sequence = sequence;
                this->value = record.value;

                // Move to next record
                address = next(address);
                sequence = next(sequence);
            }

            LOG.log(F("Last valid record found with sequence"), this->sequence, '=', this->address.sector, '@', this->address.offset);
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

    const value_t* operator->() const {
        return &this->value;
    }

    void write(const value_t& value) {
        address_t address = next(this->address);
        sequence_t sequence = next(this->sequence);

        // If we write to an sector for the first time, we have to erase it
        if (address.sector != this->address.sector) {
            flash_erase_sector(address.sector);
        }

        record_t record;
        record.sequence = sequence;
        record.value = value;
        record.checksum = checksum(record);

        LOG.log("Persisting record", sequence, "to", address.sector, "@", address.offset);
        flash_write(address,
                    record);

        this->address = address;
        this->sequence = sequence;
        this->value = value;
    }

private:
    inline static void flash_erase_sector(const uint16_t& sect) {
        WDT_FEED();

        const auto& ret = spi_flash_erase_sector(flash_addr / SPI_FLASH_SEC_SIZE + sect);
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Erase failed:", sect);
        }
    }

    inline static void flash_write(const address_t& addr,
                                   const record_t& record) {
        WDT_FEED();

        const auto& ret = spi_flash_write(flash_addr + addr.sector * SPI_FLASH_SEC_SIZE + addr.offset,
                                          (uint32_t*) &record,
                                          sizeof(record_t));
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Write failed:", addr.sector, "@", addr.offset);
        }
    }

    inline static void flash_read(const address_t& addr,
                                  record_t& record) {
        WDT_FEED();

        const auto& ret = spi_flash_read(flash_addr + addr.sector * SPI_FLASH_SEC_SIZE + addr.offset,
                                         (uint32_t*) &record,
                                         sizeof(record_t));
        if (ret != SPI_FLASH_RESULT_OK) {
            LOG.log("Flash: Read failed:", addr.sector, "@", addr.offset);
        }
    }

    /**
     * Finds the next address after the given one.
     */
    inline static address_t next(const address_t& address) {
        address_t result;
        result.sector = address.sector;
        result.offset = address.offset + sizeof(record_t);

        // If the next record would protrude into the next sector, align it to the sector start
        if (result.offset + sizeof(record_t) > SPI_FLASH_SEC_SIZE) {
            result.sector = (result.sector + 1) % (flash_size / SPI_FLASH_SEC_SIZE);
            result.offset = 0;
        }

        return result;
    }

    /**
     * Finds the next sequence after the given one.
     */
    inline static sequence_t next(const sequence_t& sequence) {
        return (sequence + 1) % (sequence_t) - 2;
    }

    /**
     * Calculate a device specific checksum for a record.
     */
    inline static Checksum::value_t checksum(const record_t& record) {
        return Checksum(hash(DEVICE)) << record;
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

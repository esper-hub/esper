#include "Info.h"

const char INFO_NAME[] = "info";

Info::Info(Device* const device)
        : Service(device) {
    // Publish status regularly
    this->timer.initializeMs(60000, TimerDelegate(&Info::publish, this));

    // Record time then starting up
    this->startupTime = RTC.getRtcSeconds();

    // Read bootloader config
    const rboot_config rbootconf = rboot_get_config();

    LOG.log("Device:", DEVICE);
    LOG.log("SDK Version:", system_get_sdk_version());
    LOG.log("Boot Version:", system_get_boot_version());
    LOG.log("Boot Mode:", system_get_boot_mode());
    LOG.log("ESPer Version:", VERSION);
    LOG.log("Free Heap:", system_get_free_heap_size());
    LOG.log("CPU Frequency (MHz):", system_get_cpu_freq());
    LOG.log("System Chip ID:", String(system_get_chip_id(), 16));
    LOG.log("SPI Flash ID:", String(spi_flash_get_id(), 16));
    LOG.log("ROM Selected:", rbootconf.current_rom);
    LOG.log("ROM Slot 0:", rbootconf.roms[0]);
    LOG.log("ROM Slot 1:", rbootconf.roms[1]);
}

Info::~Info() {
}

void Info::onStateChanged(const State& state) {
    switch (state) {
        case State::CONNECTED: {
            // Record time when going online
            this->connectTime = RTC.getRtcSeconds();

            this->timer.start();
            this->publish();

            break;
        }

        case State::DISCONNECTED: {
            this->timer.stop();

            break;
        }
    }
}

void Info::publish() {
    LOG.log("Publishing device info");

    this->device->publish("/info",
            StringSumHelper("") +
            "DEVICE=" + DEVICE + "\n" +
            "ESPER=" + VERSION + "\n" +
            "SDK=" + system_get_sdk_version() + "\n" +
            "BOOT=v" + String(system_get_boot_version()) + "\n" +
            "CHIP=" + String(system_get_chip_id(), 16) + "\n" +
            "FLASH=" + String(spi_flash_get_id(), 16) + "\n" +
            "ROM=" + String(rboot_get_current_rom()) + "\n" +
            "TIME_STARTUP=" + String(this->startupTime) + "\n" +
            "TIME_CONNECT=" + String(this->connectTime) + "\n" +
            "TIME_CURRENT=" + String(RTC.getRtcSeconds()) + "\n",
            true);
}

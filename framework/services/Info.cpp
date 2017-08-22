#include "Info.h"
#include "../Device.h"


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

    StaticJsonBuffer<1024> json;

    auto& info = json.createObject();
    info.set("device", DEVICE);
    info.set("chip_id", String(system_get_chip_id(), 16));
    info.set("flash_id", String(spi_flash_get_id(), 16));

    auto& version = info.createNestedObject("version");
    version.set("esper", VERSION);
    version.set("sdk", system_get_sdk_version());
    version.set("boot", system_get_boot_version());

    auto& boot = info.createNestedObject("boot");
    boot.set("rom", rboot_get_current_rom());

    auto& time = info.createNestedObject("time");
    time.set("startup", this->startupTime);
    time.set("connect", this->connectTime);
    time.set("updated", RTC.getRtcSeconds());

    auto& network = info.createNestedObject("network");
    network.set("mac", WifiStation.getMAC());
    network.set("ip", WifiStation.getIP().toString());
    network.set("mask", WifiStation.getNetworkMask().toString());
    network.set("gateway", WifiStation.getNetworkGateway().toString());

    auto& wifi = info.createNestedObject("wifi");
    wifi.set("ssid", this->device->getWifi().getCurrentSSID());
    wifi.set("bssid", this->device->getWifi().getCurrentBSSID());
    wifi.set("rssi", WifiStation.getRssi());
    wifi.set("channel", WifiStation.getChannel());

    String payload;
    info.prettyPrintTo(payload);

    this->device->publish(Device::TOPIC_BASE + "/info", payload, true);
}

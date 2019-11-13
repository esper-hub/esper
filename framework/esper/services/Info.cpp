#include "Info.h"
#include "../Device.h"

#include <ArduinoJson.h>


const char INFO_NAME[] = "info";

Info::Info(Device* const device)
        : Service(device) {
    // Publish status regularly
    this->timer.initializeMs(60000, std::bind(&Info::publish, this));

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

#ifdef INFO_HTTP_ENABLED
    http.listen(INFO_HTTP_PORT);
    http.paths.set("/", HttpPathDelegate(&Info::onHttpIndex, this));
#endif
}

Info::~Info() {
}

void Info::onStateChanged(const State& state) {
    switch (state) {
        case State::CONNECTED: {
            // Record time when going online
            this->connectTime = RTC.getRtcSeconds();

            this->publish();
            this->timer.start();

            break;
        }

        case State::DISCONNECTED: {
            this->timer.stop();

            break;
        }
    }
}

String Info::dump() const {
    LOG.log("Publishing device info");

    StaticJsonDocument<1024> doc;


    doc["device"] = DEVICE;
    doc["chip_id"] = String(system_get_chip_id(), 16);
    doc["flash_id"] = String(spi_flash_get_id(), 16);

    doc["version"]["esper"] = VERSION;
    doc["version"]["sdk"] = system_get_sdk_version();
    doc["version"]["boot"] = system_get_boot_version();

    doc["boot"]["rom"] = rboot_get_current_rom();

    doc["time"]["startup"] = this->startupTime;
    doc["time"]["connect"] = this->connectTime;
    doc["time"]["updated"] = RTC.getRtcSeconds();

    doc["network"]["mac"] = WifiStation.getMAC();
    doc["network"]["ip"] = WifiStation.getIP().toString();
    doc["network"]["mask"] = WifiStation.getNetworkMask().toString();
    doc["network"]["gateway"] = WifiStation.getNetworkGateway().toString();

    doc["wifi"]["ssid"] = this->device->getWifi().getCurrentSSID();
    doc["wifi"]["bssid"] = this->device->getWifi().getCurrentBSSID();
    doc["wifi"]["rssi"] = WifiStation.getRssi();
    doc["wifi"]["channel"] = WifiStation.getChannel();

    for (unsigned int i = 0; i < this->device->getServices().count(); i++) {
        doc["services"].add(this->device->getServices().at(i)->getName());
    }

    for (unsigned int i = 0; i < this->device->getSubscriptions().count(); i++) {
        doc["endpoints"].add(this->device->getSubscriptions().keyAt(i));
    }

    String payload;
    serializeJson(doc, payload);

    return payload;
}

void Info::publish() {
    this->device->publish(Device::TOPIC_BASE + "/info", this->dump(), true);
}

#ifdef INFO_HTTP_ENABLED
void Info::onHttpIndex(HttpRequest &request, HttpResponse &response) {
    response.setContentType("application/json");
    response.sendString(this->dump());
}
#endif

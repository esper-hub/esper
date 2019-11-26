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

    LOG.log(F("Device:"), DEVICE);
    LOG.log(F("SDK Version:"), system_get_sdk_version());
    LOG.log(F("Boot Version:"), system_get_boot_version());
    LOG.log(F("Boot Mode:"), system_get_boot_mode());
    LOG.log(F("ESPer Version:"), VERSION);
    LOG.log(F("Free Heap:"), system_get_free_heap_size());
    LOG.log(F("CPU Frequency (MHz):"), system_get_cpu_freq());
    LOG.log(F("System Chip ID:"), String(system_get_chip_id(), 16));
    LOG.log(F("SPI Flash ID:"), String(spi_flash_get_id(), 16));
    LOG.log(F("ROM Selected:"), rbootconf.current_rom);
    LOG.log(F("ROM Slot 0:"), rbootconf.roms[0]);
    LOG.log(F("ROM Slot 1:"), rbootconf.roms[1]);

#if HTTP_ENABLED
    auto resource = new HttpResource();
    resource->onRequestComplete = [=](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
        response.setContentType(F("application/json"));
        response.sendString(this->dumpJson());
        return 0;
    };
    this->device->registerResource(F("/"), resource);
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

String Info::dumpJson() const {
    LOG.log(F("Publishing device info"));

    StaticJsonDocument<1024> doc;


    doc[F("device")] = DEVICE;
    doc[F("chip_id")] = String(system_get_chip_id(), 16);
    doc[F("flash_id")] = String(spi_flash_get_id(), 16);

    doc[F("version")][F("esper")] = VERSION;
    doc[F("version")][F("sdk")] = system_get_sdk_version();
    doc[F("version")][F("boot")] = system_get_boot_version();

    doc[F("boot")][F("rom")] = rboot_get_current_rom();

    doc[F("time")][F("startup")] = this->startupTime;
    doc[F("time")][F("connect")] = this->connectTime;
    doc[F("time")][F("updated")] = RTC.getRtcSeconds();

    doc[F("network")][F("mac")] = WifiStation.getMAC();
    doc[F("network")][F("ip")] = WifiStation.getIP().toString();
    doc[F("network")][F("mask")] = WifiStation.getNetworkMask().toString();
    doc[F("network")][F("gateway")] = WifiStation.getNetworkGateway().toString();

    doc[F("wifi")][F("ssid")] = this->device->getWifi().getCurrentSSID();
    doc[F("wifi")][F("bssid")] = this->device->getWifi().getCurrentBSSID();
    doc[F("wifi")][F("rssi")] = WifiStation.getRssi();
    doc[F("wifi")][F("channel")] = WifiStation.getChannel();

    for (unsigned int i = 0; i < this->device->getServices().count(); i++) {
        doc[F("services")].add(this->device->getServices().at(i)->getName());
    }

    for (unsigned int i = 0; i < this->device->getSubscriptions().count(); i++) {
        doc[F("endpoints")].add(this->device->getSubscriptions().keyAt(i));
    }

    String payload;
    serializeJson(doc, payload);

    return payload;
}

void Info::publish() {
    this->device->publish(Device::TOPIC_BASE + F("/info"), this->dumpJson(), true);
}

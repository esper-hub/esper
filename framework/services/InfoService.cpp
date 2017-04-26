#include "InfoService.h"

const char INFO_SERVICE_NAME[] = "info";

InfoService::InfoService() :
        Service() {
}

InfoService::~InfoService() {
}

void InfoService::onConnected() {
    this->publish("info",
                  StringSumHelper("") +
                  "DEVICE=" + DEVICE + "\n" +
                  "ESPER=" + VERSION + "\n" +
                  "SDK=" + system_get_sdk_version() + "\n" +
                  "BOOT=v" + String(system_get_boot_version()) + "\n" +
                  "CHIP=" + String(system_get_chip_id(), 16) + "\n" +
                  "FLASH=" + String(spi_flash_get_id(), 16) + "\n" +
                  "ROM=" + String(rboot_get_current_rom()) + "\n",
                  true);
}

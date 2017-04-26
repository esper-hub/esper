#include "Status.h"


Status::Status(Device* device) {
}

Status::~Status() {
}

void Status::onConnected() {
    StringSumHelper s;
    s += "ESPER=v" + VERSION + "\n";
    s += "SDK=v" + system_get_sdk_version() + "\n";
    s += "BOOT=v" + String(system_get_boot_version()) + "\n";
    s += "CHIP=" + String(system_get_chip_id(), 16) + "\n";
    s += "FLASH=" + String(spi_flash_get_id(), 16) + "\n";
    s += "DEVICE=" + DEVICE + "\n";
    s += "ROM=" + String(rboot_get_current_rom()) + "\n";

    this->device->publish(this->topicBase, s);
}

void onDisconnected() {
}

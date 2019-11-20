#include "Reboot.h"
#include "../Device.h"


const char REBOOT_NAME[] = "reboot";

Reboot::Reboot(Device* const device)
        : Service(device) {
    // Receive messages
    this->device->registerSubscription(MQTT_REALM + String(F("/reboot")), Device::MessageCallback(&Reboot::onRebootRequestReceived, this));
    this->device->registerSubscription(Device::TOPIC_BASE + String(F("/reboot")), Device::MessageCallback(&Reboot::onRebootRequestReceived, this));
}

Reboot::~Reboot() {
}

void Reboot::trigger() {
    LOG.log(F("Rebooting System"));
    System.restart();
}

void Reboot::onRebootRequestReceived(const String& message) {
    // Message must contain device ID to prevent accidental restarts
    if (message != String(system_get_chip_id(), 16)) {
        return;
    }

    this->trigger();
}

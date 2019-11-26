#include "Reboot.h"
#include "../Device.h"


const char REBOOT_NAME[] = "reboot";

Reboot::Reboot(Device* const device)
        : Service(device) {
    // Receive messages
    this->device->registerSubscription(MQTT_REALM + String(F("/reboot")), Device::MessageCallback(&Reboot::onRebootRequestReceived, this));
    this->device->registerSubscription(Device::TOPIC_BASE + String(F("/reboot")), Device::MessageCallback(&Reboot::onRebootRequestReceived, this));

#if HTTP_ENABLED
    auto resource = new HttpResource();
    resource->onRequestComplete = [=](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
        if(request.method != HTTP_POST) {
            response.code = HTTP_STATUS_BAD_REQUEST;
            return 0;
        }

        this->trigger();
        return 0;
    };
    this->device->registerResource(F("/reboot"), resource);
#endif
}

Reboot::~Reboot() {
}

void Reboot::trigger(unsigned int deferMillis) {
    LOG.log(F("Rebooting System"));
    System.restart(deferMillis);
}

void Reboot::onRebootRequestReceived(const String& message) {
    // Message must contain device ID to prevent accidental restarts
    if (message != String(system_get_chip_id(), 16)) {
        return;
    }

    this->trigger();
}

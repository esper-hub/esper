#include "Heartbeat.h"
#include "../Device.h"


const char HEARTBEAT_NAME[] = "heartbeat";

Heartbeat::Heartbeat(Device* const device)
        : Service(device) {
    // Receive heartbeat messages
    this->device->registerSubscription(HEARTBEAT_TOPIC, Device::MessageCallback(&Heartbeat::onMessageReceived, this));

    // Reboot the system if heartbeat was missing
    this->timer.initializeMs(120000, TimerDelegate(&Device::reboot, device));
}

Heartbeat::~Heartbeat() {
}

void Heartbeat::onStateChanged(const State& state) {
    switch (state) {
        case State::CONNECTED: {
            LOG.log("Start awaiting heartbeats");
            this->timer.start();

            break;
        }

        case State::DISCONNECTED: {
            // Heartbeats are likely to miss if disconnected
            LOG.log("Stop awaiting heartbeats");
            this->timer.stop();

            break;
        }
    }
}

void Heartbeat::onMessageReceived(const String& topic, const String& message) {
    // Handle incoming heartbeat
    LOG.log("Heartbeat");
    this->timer.restart();
}

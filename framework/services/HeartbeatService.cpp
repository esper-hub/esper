#include "HeartbeatService.h"

const char HEARTBEAT_SERVICE_NAME[] = "heartbeat";

HeartbeatService::HeartbeatService(const uint16_t& interval) :
        Service() {
    // Reboot if a message was not received on the second interval
    this->timer.initializeMs(interval * 2, TimerDelegate(&HeartbeatService::onTimeout, this));
}

HeartbeatService::~HeartbeatService() {
}

void HeartbeatService::initialize(MessageDispatcher* const dispatcher,
                                  MessagePublisher* const publisher) {
    // Listen for heartbeat messages
    dispatcher->subscribe("heartbeat", MessageDispatcher::MessageCallback(&HeartbeatService::onMessage, this));
}

void HeartbeatService::onConnected() {
    // Start awaiting heartbeats
    this->timer.start();
}

void HeartbeatService::onDisconnected() {
    // Heartbeats are likely to miss if disconnected
    this->timer.stop();
}

void HeartbeatService::onTimeout() {
    LOG.log("Restarting system because of missing heartbeat");
    System.restart();
}

void HeartbeatService::onMessage(const String& topic, const String& message) {
    // Handle incoming heartbeat
    LOG.log("Heartbeat");
    this->timer.restart();
}

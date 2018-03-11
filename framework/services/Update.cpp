#include "Update.h"
#include "../Device.h"


const char UPDATE_NAME[] = "update";

#define UPDATE_URL_VERSION (( UPDATE_URL "/" DEVICE ".version" ))
#define UPDATE_URL_ROM_0 (( UPDATE_URL "/" DEVICE ".rom0" ))
#define UPDATE_URL_ROM_1 (( UPDATE_URL "/" DEVICE ".rom1" ))


Update::Update(Device* device) :
        Service(device),
        updater(nullptr) {
    // Receive update messages
    this->device->registerSubscription(MQTT_REALM + String("/update"), Device::MessageCallback(&Update::onGlobalUpdateRequestReceived, this));
    this->device->registerSubscription(Device::TOPIC_BASE + String("/update"), Device::MessageCallback(&Update::onDeviceUpdateRequestReceived, this));

    // Check for updates regularly
    this->checkTimer.initializeMs(UPDATE_INTERVAL * 1000, TimerDelegate(&Update::checkUpdate, this));

#if UPDATE_DELAY != 0
    this->delayTimer.initializeMs(random(UPDATE_DELAY * 1000), TimerDelegate(&Update::checkUpdate, this));
#endif
}

Update::~Update() {
}

void Update::onStateChanged(const State& state) {
    switch (state) {
        case State::CONNECTED: {
            this->checkTimer.start();

            break;
        }

        case State::DISCONNECTED: {
            this->checkTimer.stop();

#if UPDATE_DELAY != 0
            this->delayTimer.stop();
#endif

            break;
        }
    }
}

void Update::checkUpdate() {
    LOG.log("Downloading version file");
    this->http.downloadString(UPDATE_URL_VERSION, RequestCompletedDelegate(&Update::onVersionReceived, this));
}

void Update::onGlobalUpdateRequestReceived(const String& topic, const String& message) {
    LOG.log("Request for global update received");

#if UPDATE_DELAY != 0
    this->delayTimer.start(false);
#else
    this->checkUpdate();
#endif
}

void Update::onDeviceUpdateRequestReceived(const String& topic, const String& message) {
    LOG.log("Request for device specific update received");
    this->checkUpdate();
}

int Update::onVersionReceived(HttpConnection& client, bool successful) {
    if (!successful) {
        LOG.log("Version download failed");
        return -1;
    }

    LOG.log("Got version file");

    // Get the first line received
    String version = client.getResponseString();
    version = version.substring(0, version.indexOf('\n') - 1);

    // Compare latest version with current one
    if (version == VERSION) {
        LOG.log("Already up to date");
        return -1;
    }

    LOG.log("Remote version differs - updating...");

    // Select rom slot to flash
    const rboot_config bootconf = rboot_get_config();

    // Ensure we have a clean updater
    if (this->updater) delete this->updater;
    this->updater = new rBootHttpUpdate();

    // Configure updater with items to flash
    if (bootconf.current_rom == 0) {
        LOG.log("Flashing", UPDATE_URL_ROM_1, "to", String(bootconf.roms[1], 16));
        this->updater->addItem(bootconf.roms[1], UPDATE_URL_ROM_1);
        this->updater->switchToRom(1);
    } else {
        LOG.log("Flashing", UPDATE_URL_ROM_0, "to", String(bootconf.roms[0], 16));
        this->updater->addItem(bootconf.roms[0], UPDATE_URL_ROM_0);
        this->updater->switchToRom(0);
    }

    // Start update
    LOG.log("Downloading update");
    this->updater->start();

    return 0;
}

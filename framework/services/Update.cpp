#include "Update.h"

const char UPDATE_NAME[] = "update";

#define UPDATE_URL_VERSION (( UPDATE_URL "/" DEVICE ".version" ))
#define UPDATE_URL_ROM_0 (( UPDATE_URL "/" DEVICE ".rom0" ))
#define UPDATE_URL_ROM_1 (( UPDATE_URL "/" DEVICE ".rom1" ))


Update::Update(Device* device) :
        Service(device),
        updater(nullptr) {
    // REceive update messages
    this->device->registerSubscription(UPDATE_TOPIC, Device::MessageCallback(&Update::onUpdateRequestReceived, this));

    // Check for updates regularly
    this->timer.initializeMs(UPDATE_INTERVAL, TimerDelegate(&Update::checkUpdate, this));
}

Update::~Update() {
}

void Update::checkUpdate() {
    LOG.log("Downloading version file");
    this->http.downloadString(UPDATE_URL_VERSION, HttpClientCompletedDelegate(&Update::onVersionReceived, this));
}

void Update::onUpdateRequestReceived(const String& topic, const String& message) {
    LOG.log("Request for update received");
    this->checkUpdate();
}

void Update::onVersionReceived(HttpClient& client, bool successful) {
    if (!successful) {
        LOG.log("Version download failed");
        return;
    }

    LOG.log("Got version file");

    // Get the first line received
    String version = client.getResponseString();
    version = version.substring(0, version.indexOf('\n') - 1);

    // Compare latest version with current one
    if (version == VERSION) {
        LOG.log("Already up to date");
        return;
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
}

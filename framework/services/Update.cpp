#include "Update.h"


#define UPDATER_URL_ROM(slot) (( UPDATER_URL "/" DEVICE ".rom" slot ))
#define UPDATER_URL_VERSION (( UPDATER_URL "/" DEVICE ".version" ))


Update::Update(Device* device) :
        Service(device) {
    this->timer.initializeMs(UPDATER_INTERVAL, TimerDelegate(&update));
}

virtual Update::~Update() {
}

void Update::onConnected() {
    this->subscribe(UPDATER_TOPIC, Device::MessageCallback(&Update::onUpdateRequestReceived, this));
}

void Update::onDisconnected() {
}

void Update::checkUpdate() {
    LOG.log("Downloading version file");
    this->http.downloadString(UPDATER_URL_VERSION, HttpClientCompletedDelegate(&Update::onVersionReceived, this));
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
        LOG.log("Flashing", UPDATER_URL_ROM("1"), "to", String(bootconf.roms[1], 16));
        this->updater->addItem(bootconf.roms[1], UPDATER_URL_ROM("1"));
        this->updater->switchToRom(1);
    } else {
        LOG.log("Flashing", UPDATER_URL_ROM("0"), "to", String(bootconf.roms[0], 16));
        this->updater->addItem(bootconf.roms[0], UPDATER_URL_ROM("0"));
        this->updater->switchToRom(0);
    }

    // Start update
    LOG.log("Downloading update");
    this->updater->start();
}

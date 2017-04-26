#include "UpdateService.h"


#define UPDATER_URL_ROM(slot) (( UPDATER_URL "/" DEVICE ".rom" slot ))
#define UPDATER_URL_VERSION (( UPDATER_URL "/" DEVICE ".version" ))


const char UPDATE_SERVICE_NAME[] = "update";


UpdateService::UpdateService(const uint16_t& interval) :
        Service() {
    // Trigger update attempt on interval
    this->timer.initializeMs(interval, TimerDelegate(&UpdateService::onTimeout, this));
}

UpdateService::~UpdateService() {
}

void UpdateService::initialize(MessageDispatcher* const dispatcher,
                               MessagePublisher* const publisher) {
    // Listening for update requests
    dispatcher->subscribe("update", Device::MessageCallback(&UpdateService::onMessage, this));
}

void UpdateService::onTimeout() {
    this->triggerUpdate();
}

void UpdateService::onMessage(const String& topic, const String& message) {
    this->triggerUpdate();
}

void UpdateService::triggerUpdate() {
    LOG.log("Starting");

    LOG.log("Downloading version file");
    this->checkHttpClient.downloadString(UPDATER_URL_VERSION, HttpClientCompletedDelegate(&UpdateService::onCheckCompleted, this));
}

void UpdateService::onCheckCompleted(HttpClient& client, bool successful) {
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

    // Add items to flash
    if (bootconf.current_rom == 0) {
        LOG.log("Flashing", UPDATER_URL_ROM("1"), "to", String(bootconf.roms[1], 16));
        this->currentUpdater->addItem(bootconf.roms[1], UPDATER_URL_ROM("1"));
        this->currentUpdater->switchToRom(1);
    } else {
        LOG.log("Flashing", UPDATER_URL_ROM("0"), "to", String(bootconf.roms[0], 16));
        this->currentUpdater->addItem(bootconf.roms[0], UPDATER_URL_ROM("0"));
        this->currentUpdater->switchToRom(0);
    }

    // Start update
    LOG.log("Downloading update");
    this->currentUpdater->start();
}

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
    this->device->registerSubscription(MQTT_REALM + String("/update"), Device::MessageCallback(&Update::onUpdateRequestReceived, this));
    this->device->registerSubscription(Device::TOPIC_BASE + String("/update"), Device::MessageCallback(&Update::onUpdateRequestReceived, this));

    // Check for updates regularly
    this->timer.initializeMs(UPDATE_INTERVAL, TimerDelegate(&Update::checkUpdate, this));
}

Update::~Update() {
}

void Update::checkUpdate() {
    LOG.log("Downloading version file");
    this->http_version.downloadString(UPDATE_URL_VERSION, RequestCompletedDelegate(&Update::onVersionReceived, this));
}

void Update::onUpdateRequestReceived(const String& topic, const String& message) {
    LOG.log("Request for update received");
    this->checkUpdate();
}

int Update::onVersionReceived(HttpConnection& client, bool successful) {
    if (!successful) {
        LOG.log("Version download failed, aborting.");
        return -1;
    }
    if (client.getResponse()->code != 200) {
        LOG.log("Version download failed with HTTP response code", client.getResponse()->code, "!= 200, aborting.");
        return -1;
    }

    LOG.log("Got version file");

    // Get the first line received
    String version = client.getResponseString();
    version = version.substring(0, version.indexOf('\n') - 1);

    if (version == "") {
        LOG.log("Got empty version string, aborting.");
        return -1;
    }

    // Compare latest version with current one
    if (version == VERSION) {
        LOG.log("Already up to date!");
        return -1;
    }

    LOG.log("Remote version is different!");
    this->checkImageHeader();
}

void Update::checkImageHeader() {
    const rboot_config bootconf = rboot_get_config();

    HttpRequest* request;
    if (bootconf.current_rom == 0) {
        request = this->http_image_check.request(UPDATE_URL_ROM_1);
    } else {
        request = this->http_image_check.request(UPDATE_URL_ROM_0);
    }
    request->setMethod(HTTP_HEAD);
    request->onRequestComplete(RequestCompletedDelegate(&Update::onImageHeaderReceived, this));
    if (this->http_image_check.send(request)) {
       LOG.log("head request queued");
    } else {
       LOG.log("head request errored");
    }
    LOG.log("Requesting image with HTTP HEAD request");
}

int Update::onImageHeaderReceived(HttpConnection& client, bool successful) {
    if (!successful) {
        LOG.log("HEAD request for firmware image failed, aborting.");
        return -1;
    }
    if (client.getResponse()->code != 200) {
        LOG.log("HEAD request for firmware image failed with HTTP response code", client.getResponse()->code, "!= 200, aborting.");
        return -1;
    }

    LOG.log("Firmware image HEAD requests looks good, continuing.");
    this->beginUpdate();

    return 0;
}

void Update::beginUpdate() {
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
    LOG.log("Beginning update...");
    this->updater->start();
}

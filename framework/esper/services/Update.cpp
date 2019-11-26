#include "Update.h"
#include "../Device.h"


const char UPDATE_NAME[] = "update";

const char* UPDATE_URL_VERSION = UPDATE_URL "/" DEVICE ".version";
const char* UPDATE_URL_ROM[] = {
    UPDATE_URL "/" DEVICE ".rom0",
    UPDATE_URL "/" DEVICE ".rom1",
};


Update::Update(Device* device) :
        Service(device),
        device(device) {
    // Receive update messages
    this->device->registerSubscription(MQTT_REALM + String(F("/update")), Device::MessageCallback(&Update::onGlobalUpdateRequestReceived, this));
    this->device->registerSubscription(Device::TOPIC_BASE + String(F("/update")), Device::MessageCallback(&Update::onDeviceUpdateRequestReceived, this));

    // Check for updates regularly
    this->checkTimer.initializeMs(UPDATE_INTERVAL * 1000, std::bind(&Update::check, this));

#if UPDATE_DELAY != 0
    this->delayTimer.initializeMs(random(UPDATE_DELAY * 1000), std::bind(&Update::check, this));
#endif

#if HTTP_ENABLED
    auto resource = new HttpResource();
    resource->onHeadersComplete = [=](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
        Target* target = new Target(Update::target());

        LOG.log(F("Uploading to"), String(target->stream->getStartAddress(), 16));

        connection.userData = target;
        return 0;
    };
    resource->onBody = [=](HttpServerConnection& connection, HttpRequest& request, const char* at, int length) -> int {
        Target* target = static_cast<Target*>(connection.userData);

        const int written = target->stream->write((uint8_t*)at, length);
        if (written != length) {
            LOG.log(F("Uploading failed"));
            return 1;
        }

        return 0;
    };
    resource->onRequestComplete = [=](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
        Target* target = static_cast<Target*>(connection.userData);

        LOG.log(F("Uploading finished"));

        target->stream->close();

        System.queueCallback([=]() {
            this->apply(target->slot);
        });

        return 0;
    };
    this->device->registerResource(F("/update"), resource);
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

void Update::check() {
    LOG.log(F("Downloading version file"));
    this->http.downloadString(UPDATE_URL_VERSION, [=](HttpConnection& client, bool successful) -> int {
        if (!successful) {
            LOG.log(F("Version download failed"));
            return -1;
        }

        LOG.log(F("Got version file"));

        // Get the first line received
        String version = client.getResponse()->getBody();
        version = version.substring(0, version.indexOf('\n') - 1);

        // Compare latest version with current one
        if (version == VERSION) {
            LOG.log(F("Already up to date"));
            return -1;
        }

        LOG.log(F("Remote version differs - updating..."));
        System.queueCallback(TaskDelegate(&Update::download, this));

        return 0;
    });
}

void Update::download() {
    const auto target = Update::target();
    const auto& url = UPDATE_URL_ROM[target.slot];

    LOG.log(F("Flashing"), url, F("to"), String(target.stream->getStartAddress(), 16));

    HttpRequest* request = new HttpRequest(url);
    request->setMethod(HTTP_GET);
    request->setResponseStream(target.stream);
    request->onRequestComplete([=](HttpConnection& client, bool success) -> int {
        if (!success) {
            LOG.log(F("Downloading update failed"));
            return -1;
        }

        LOG.log(F("Downloading update complete - applying"));
        System.queueCallback([=]() {
            this->apply(target.slot);
        });

        return 0;
    });

    // Start update
    LOG.log(F("Downloading update"));
    this->http.send(request);
}

void Update::apply(uint8_t slot) {
    LOG.log(F("Switching to ROM slot"), slot);
    rboot_set_current_rom(slot);

    this->device->triggerReboot(1000);
}

void Update::onGlobalUpdateRequestReceived(const String& message) {
    LOG.log(F("Request for global update received"));

#if UPDATE_DELAY != 0
    this->delayTimer.start(false);
#else
    this->check();
#endif
}

void Update::onDeviceUpdateRequestReceived(const String& message) {
    LOG.log(F("Request for device specific update received"));
    this->download();
}

Update::Target Update::target() {
    // Read currently running boot config from flash
    const rboot_config bootconf = rboot_get_config();

    // Find ROM slot contrary to the currently booted rom slot
    uint8_t slot = (bootconf.current_rom == 0) ? 1 : 0;

    return {
        .stream = new RbootOutputStream(bootconf.roms[slot], 0),
        .slot = slot,
    };
}

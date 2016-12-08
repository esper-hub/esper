#include "Updater.h"
#include "../util/Logger.h"


#define UPDATER_URL_ROM(slot) (( UPDATER_URL "/" DEVICE ".rom" slot ))
#define UPDATER_URL_VERSION (( UPDATER_URL "/" DEVICE ".version" ))


const Logger LOG = Logger("Updater");


static HttpClient http;
static rBootHttpUpdate updater;


void onVersion(HttpClient& client, bool successful) {
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
        updater.addItem(bootconf.roms[1], UPDATER_URL_ROM("1"));
        updater.switchToRom(1);
    } else {
        LOG.log("Flashing", UPDATER_URL_ROM("0"), "to", String(bootconf.roms[0], 16));
        updater.addItem(bootconf.roms[0], UPDATER_URL_ROM("0"));
        updater.switchToRom(0);
    }

    // Start update
    LOG.log("Downloading update");
    updater.start();
}


void update() {
    LOG.log("Starting");

    LOG.log("Downloading version file");
    http.downloadString(UPDATER_URL_VERSION, onVersion);
}

#ifndef UPDATER_H
#define UPDATER_H

#ifndef UPDATE_TOPIC
#error "UPDATE_TOPIC not defined"
#endif
#ifndef UPDATE_INTERVAL
#define UPDATE_INTERVAL 3600
#endif

#include "Service.h"


extern const char UPDATE_NAME[];


class Update : public Service<UPDATE_NAME> {
public:
    Update(Device* device);
    virtual ~Update();

    void checkUpdate();

private:
    void onUpdateRequestReceived(const String& topic, const String& message);
    int onVersionReceived(HttpConnection& client, bool successful);
    void checkImageHeader();
    int onImageHeaderReceived(HttpConnection& client, bool successful);
    void beginUpdate();

    Timer timer;

    HttpClient http_version;
    HttpClient http_image_check;

    rBootHttpUpdate* updater;
};

#endif

#ifndef SERVICE_H
#define SERVICE_H

#include "../util/Logger.h"
#include "../Device.h"

template<const char* const name>
class Service : public ServiceBase {
protected:
    static const Logger LOG;

    Service(Device* const device) :
            device(device) {
    }

    virtual ~Service() {
    }

public:
    virtual const char* getName() const {
        return name;
    }

    virtual void onStateChanged(const Service::State& state) {
    }

protected:
    Device* const device;
};


template<const char* const name>
const Logger Service<name>::LOG = Logger(name);

#endif

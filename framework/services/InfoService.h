#ifndef INFO_SERVICE_H
#define INFO_SERVICE_H

#include "Service.h"


extern const char INFO_SERVICE_NAME[];


class InfoService : public Service<INFO_SERVICE_NAME> {
public:
    explicit InfoService();
    virtual ~InfoService();

protected:
    virtual void onConnected();
};


#endif

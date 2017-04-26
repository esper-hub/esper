#ifndef FEATURE_SERVICE_H
#define FEATURE_SERVICE_H

#include "Service.h"

#include "../util/Logger.h"


class FeatureBase {
    friend class FeatureService;
protected:
    explicit FeatureBase();
    virtual ~FeatureBase();

public:
    virtual const char* getName() const = 0;

private:
    virtual void registerSubscriptions(MessageDispatcher* dispatcher) = 0;
    virtual void publishCurrentState() = 0;
};


class FeatureService : public ServiceBase {
public:
    explicit FeatureService(FeatureBase* const feature);

    virtual ~FeatureService();

    virtual const char* getName() const;

protected:
    virtual void initialize(MessageDispatcher* const dispatcher,
                            MessagePublisher* const publisher);
    virtual void onConnected();

private:
    FeatureBase* const feature;
};


#endif

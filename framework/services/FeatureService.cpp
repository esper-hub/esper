#include "FeatureService.h"


FeatureBase::FeatureBase() {
}

FeatureBase::~FeatureBase() {
}


FeatureService::FeatureService(FeatureBase* const feature) :
        ServiceBase(),
        feature(feature) {
}

FeatureService::~FeatureService() {
}

const char* FeatureService::getName() const {
    return this->feature->getName();
}

void FeatureService::initialize(MessageDispatcher* const dispatcher,
                                MessagePublisher* const publisher) {
    PrefixedMessageDispatcher prefixedDispatcher(Device::id() + "/" + this->feature->getName(),
                                                 dispatcher,
                                                 publisher);
    this->feature->registerSubscriptions(prefixedDispatcher);
}

void FeatureService::onConnected() {
    this->feature->publishCurrentState();
}

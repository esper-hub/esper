#ifndef BUTTON_H
#define BUTTON_H

#include "Feature.h"
#include "../util/Observed.h"
#include "../util/Damper.h"


template<const char* const name, uint16_t gpio, bool inverted, uint16_t damper_time = 100>
class Button : public Feature<name> {
    constexpr static const char* const ON = "1";
    constexpr static const char* const OFF = "0";

protected:
    using Feature<name>::LOG;

public:
    using Callback = Observed<bool>::Callback;

    Button(Device* const device,
           const Callback& callback) :
            Feature<name>(device),
            state(false, callback),
            damper()  {
        attachInterrupt(gpio, Delegate<void()>(&Button::onInterrupt, this), CHANGE);
    }

protected:
    virtual void publishCurrentState() {
        LOG.log(F("Current state:"), this->state);

        this->publish(F(""), this->state ? ON : OFF, true);
    }

    virtual bool onEdge(const bool& edge) = 0;

private:
    void onInterrupt()  {
        const bool value = (digitalRead(gpio) == !inverted);

        System.queueCallback([this, value]() {
            if(this->damper.isDamped()) {
                return;
            }

            const bool state = this->onEdge(value);

            if (state != this->state) {
                this->state.set(state);
                this->publishCurrentState();
            }
        });
    }

    Observed<bool> state;
    Damper<damper_time> damper;
};


#endif

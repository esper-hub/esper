#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include "Button.h"


template<const char* const name, uint16_t gpio, bool inverted = true, bool activeEdge = true, uint16_t damper_time = 100>
class ToggleButton : public Button<name, gpio, inverted, damper_time> {
protected:
    using Button<name, gpio, inverted>::LOG;

public:
    using Callback = typename Button<name, gpio, inverted>::Callback;

    ToggleButton(Device* const device,
                 const Callback& callback) :
            Button<name, gpio, inverted>(device, callback),
            state(false) {
    }

protected:
    virtual bool onEdge(const bool& edge) {
        if (edge == activeEdge) {
            this->state = !this->state;
        }
        return this->state;
    }

private:
    bool state;
};


#endif

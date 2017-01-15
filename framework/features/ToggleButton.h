#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include "Button.h"


template<const char* name, uint16_t gpio, bool inverted = true, bool activeEdge = true>
class ToggleButton : public Button<name, gpio, inverted> {

public:
    using Callback = typename Button<name, gpio, inverted>::Callback;

    ToggleButton(Device* device,
                 const Callback& callback) :
            Button<name, gpio, inverted>(device, callback),
            state(false) {
    }

protected:
    virtual bool onEdge(const bool& edge) {
        if (edge == activeEdge) {
            return (this->state = !this->state);
        }
    }

private:
    bool state;
};


#endif

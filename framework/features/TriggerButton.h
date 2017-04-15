#ifndef TRIGGERBUTTON_H
#define TRIGGERBUTTON_H

#include "Button.h"


template<const char* const name, uint16_t gpio, bool inverted=true>
class TriggerButton : public Button<name, gpio, inverted> {

public:
    using Callback = typename Button<name, gpio, inverted>::Callback;

    TriggerButton(Device* const device,
                  const Callback& callback) :
            Button<name, gpio, inverted>(device, callback) {
    }

protected:
    virtual bool onEdge(const bool& edge) {
        return edge;
    }
};


#endif

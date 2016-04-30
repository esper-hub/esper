#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "Feature.h"
#include "OnOffFeature.h"

constexpr static const char _SWITCH[] = "[SWITCH] ";

class Switch : public OnOffFeature<_SWITCH> {
    using OnOffFeature::OnOffFeature;
};


constexpr static const char _LED[] = "[LED] ";


class LED : public OnOffFeature<_LED, true> {
public:
    LED(HassDevice &device, const char *name, const uint16_t gpio_pin) :
            OnOffFeature(device, name, gpio_pin) {
    }
};


constexpr static const char _BUILTIN_LED[] = "[BUILTINLED] ";
class BuiltinLED : public LED, public Log<_BUILTIN_LED> {

    enum {
        LED_GPIO = 2
    };
public:

    BuiltinLED(HassDevice &device, const char *name) :
            LED(device, name, LED_GPIO) { }
};


void init() {
    HassDevice *device = new HassDevice("Steckdosenleiste!!!");

    device->add<BuiltinLED>("led");

    device->start();
}

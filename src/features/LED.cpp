#include "LED.h"

const char _LED[] = "[LED] ";

LED::LED(HassDevice &device, const char *name, const uint16_t gpio_pin) :
        OnOffFeature(device, name, gpio_pin) {
}
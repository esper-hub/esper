#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <map>
#include "MqttNode.h"
#include "Feature.h"
#include "HassDevice.h"
#include "OnOffFeature.h"

#define LED_PIN 2 // GPIO2

constexpr static const char _SWITCH[] = "switch";
class Switch : public OnOffFeature<_SWITCH> {
	using OnOffFeature::OnOffFeature;
};


constexpr static const char _LED[] = "LED";
class LED : public OnOffFeature<_LED, true> {};


void init() {
	HassDevice* device = new HassDevice("Steckdosenleiste!!!");

	Switch* plug1 = new Switch(*device, "sw1", 1);
	Switch* plug2 = new Switch(*device, "sw2", LED_PIN);
	Switch* plug3 = new Switch(*device, "sw3", 3);

	device->start();
}

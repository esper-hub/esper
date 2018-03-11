#include "../features/Light.h"
#include "../features/Socket.h"
#include "../features/ToggleButton.h"
#include "Device.h"

constexpr const char RELAY1_NAME[] = "relay1";
constexpr const uint16_t RELAY1_GPIO = 12;

constexpr const char RELAY2_NAME[] = "relay2";
constexpr const uint16_t RELAY2_GPIO = 5;

constexpr const char RELAY3_NAME[] = "relay3";
constexpr const uint16_t RELAY3_GPIO = 4;

constexpr const char RELAY4_NAME[] = "relay4";
constexpr const uint16_t RELAY4_GPIO = 15;

constexpr const char BUTTON1_NAME[] = "button1";
constexpr const uint16_t BUTTON1_GPIO = 0;

constexpr const char BUTTON2_NAME[] = "button2";
constexpr const uint16_t BUTTON2_GPIO = 9;

constexpr const char BUTTON3_NAME[] = "button3";
constexpr const uint16_t BUTTON3_GPIO = 10;

constexpr const char BUTTON4_NAME[] = "button4";
constexpr const uint16_t BUTTON4_GPIO = 14;

constexpr const char LIGHT_NAME[] = "light";
constexpr const uint16_t LIGHT_GPIO = 13;

class Sonoff4chDevice : public Device {
public:
  Sonoff4chDevice()
      : relay1(this), relay2(this), relay3(this), relay4(this),
        button1(this, decltype(button1)::Callback(
                          &Sonoff4chDevice::onBtn1Toggle, this)),
        button2(this, decltype(button2)::Callback(
                          &Sonoff4chDevice::onBtn2Toggle, this)),
        button3(this, decltype(button3)::Callback(
                          &Sonoff4chDevice::onBtn3Toggle, this)),
        button4(this, decltype(button4)::Callback(
                          &Sonoff4chDevice::onBtn4Toggle, this)),
        light(this) {
    this->add(&(this->relay1));
    this->add(&(this->relay2));
    this->add(&(this->relay3));
    this->add(&(this->relay4));

    // TODO: hook this led up to the wifi/mqtt state
    this->light.set(true);
  }

private:
  void onBtn1Toggle(const bool &state) { this->relay1.set(state); }

  void onBtn2Toggle(const bool &state) { this->relay2.set(state); }

  void onBtn3Toggle(const bool &state) { this->relay3.set(state); }

  void onBtn4Toggle(const bool &state) { this->relay4.set(state); }

  Socket<RELAY1_NAME, RELAY1_GPIO> relay1;
  Socket<RELAY2_NAME, RELAY2_GPIO> relay2;
  Socket<RELAY3_NAME, RELAY3_GPIO> relay3;
  Socket<RELAY4_NAME, RELAY4_GPIO> relay4;

  ToggleButton<BUTTON1_NAME, BUTTON1_GPIO, true> button1;
  ToggleButton<BUTTON2_NAME, BUTTON2_GPIO, true> button2;
  ToggleButton<BUTTON3_NAME, BUTTON3_GPIO, true> button3;
  ToggleButton<BUTTON4_NAME, BUTTON4_GPIO, true> button4;

  Light<LIGHT_NAME, LIGHT_GPIO> light;
};

Device *createDevice() { return new Sonoff4chDevice(); }

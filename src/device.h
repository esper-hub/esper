enum class DeviceType : int {
    ESP_DOSE,
    ESP_SCHALTER
};


#ifndef DEVICE_ESP_DOSE
#define DEVICE_ESP_DOSE DeviceType::ESP_DOSE
#endif

#ifndef DEVICE_ESP_SCHALTER
#define DEVICE_ESP_SCHALTER DeviceType::ESP_SCHALTER
#endif

#ifndef DEVICE_TYPE
#define DEVICE_TYPE DEVICE_ESP_DOSE
#endif
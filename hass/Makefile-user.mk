WIFI_SSID="cda"
WIFI_PWD="neHeeha23"
MQTT_HOST="whisky.trollhoehle.net"
MQTT_PORT=1883
REALM="/trollhoehle/lounge/"

## Local build configuration
## Parameters configured here will override default and ENV values.
## Uncomment and change examples:

## Add your source directories here separated by space
# MODULES = app
# EXTRA_INCDIR = include

## ESP_HOME sets the path where ESP tools and SDK are located.
## Windows:
# ESP_HOME = c:/Espressif

## MacOS / Linux:
# ESP_HOME = /opt/esp-open-sdk

## SMING_HOME sets the path where Sming framework is located.
## Windows:
# SMING_HOME = c:/tools/sming/Sming

## MacOS / Linux
# SMING_HOME = /opt/sming/Sming

## COM port parameter is reqruied to flash firmware correctly.
## Windows:
# COM_PORT = COM3

## MacOS / Linux:
# COM_PORT = /dev/tty.usbserial

## Com port speed
# COM_SPEED	= 115200

## Configure flash parameters (for ESP12-E and other new boards):
# SPI_MODE = dio

## SPIFFS options
DISABLE_SPIFFS = 1
# SPIFF_FILES = files


# Use load configuration
ifndef CONFIG
include $(BASEDIR)/Configuration.mk
else
include $(BASEDIR)/Configuration.mk.$(CONFIG)
endif


# Sanity checking some required variables
ifndef SMING_HOME
$(error SMING_HOME is not set. Please configure it in Configuration.mk or set in environment)
endif

ifndef ESP_HOME
$(error ESP_HOME is not set. Please configure it in Configuration.mk or set in environment)
endif


MODULES += $(BASEDIR)/framework/devices
MODULES += $(BASEDIR)/framework/features
MODULES += $(BASEDIR)/framework/managers
MODULES += $(BASEDIR)/framework/util
MODULES += $(BASEDIR)/framework

MODULES += $(BASEDIR)/devices/$(DEVICE)


# Ensure we have a version string
VERSION ?= "SNAPSHOT"


# Pass options to source code
USER_CFLAGS += -DDEVICE=\"$(DEVICE)\"

USER_CFLAGS += -DMQTT_HOST=\"$(MQTT_HOST)\"
USER_CFLAGS += -DMQTT_PORT=$(MQTT_PORT)
USER_CFLAGS += -DMQTT_REALM=\"$(MQTT_REALM)\"

ifdef HEARTBEAT_TOPIC
USER_CFLAGS += -DHEARTBEAT_TOPIC=\"$(HEARTBEAT_TOPIC)\"
endif

USER_CFLAGS += -DVERSION=\"$(VERSION)\"

USER_CFLAGS += -O3


# Disable SPIFFS
DISABLE_SPIFFS = 1


# Use alternative bootloader
RBOOT_ENABLED = 1
RBOOT_BIG_FLASH = 0
RBOOT_TWO_ROMS = 1
RBOOT_RTC_ENABLED = 1

RBOOT_LD_0 = $(BASEDIR)/rom0.ld
RBOOT_LD_1 = $(BASEDIR)/rom1.ld


# Include main makefile
include $(SMING_HOME)/Makefile-rboot.mk


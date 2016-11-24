ifndef CONFIG
$(info CONFIG is not defined - using default)
include ./Configuration.mk
else
$(info Using Configuration.mk.$(CONFIG))
include ./Configuration.mk.$(CONFIG)
endif

ifndef SMING_HOME
$(error SMING_HOME is not set. Please configure it in Configuration.mk or set in environment)
endif

ifndef ESP_HOME
$(error ESP_HOME is not set. Please configure it in Configuration.mk or set in environment)
endif

MODULES += framework/devices
MODULES += framework/features
MODULES += framework/managers
MODULES += framework/util
MODULES += framework

MODULES += devices/screen

VERSION ?= "SNAPSHOT"

USER_CFLAGS += -DMQTT_HOST=\"$(MQTT_HOST)\"
USER_CFLAGS += -DMQTT_PORT=$(MQTT_PORT)
USER_CFLAGS += -DMQTT_REALM=\"$(MQTT_REALM)\"

ifdef HEARTBEAT_TOPIC
USER_CFLAGS += -DHEARTBEAT_TOPIC=\"$(HEARTBEAT_TOPIC)\"
endif

USER_CFLAGS += -DVERSION=\"$(VERSION)\"

USER_CFLAGS += -O3

DISABLE_SPIFFS = 1

RBOOT_ENABLED = 1
RBOOT_BIG_FLASH = 0

include $(SMING_HOME)/Makefile-rboot.mk

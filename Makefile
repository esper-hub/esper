include ./Configuration.mk

ifndef SMING_HOME
$(error SMING_HOME is not set. Please configure it in Makefile-user.mk or set in environment)
endif

ifndef ESP_HOME
$(error ESP_HOME is not set. Please configure it in Makefile-user.mk or set in environment)
endif

MODULES += src/devices
MODULES += src/features
MODULES += src/managers
MODULES += src/util
MODULES += src

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

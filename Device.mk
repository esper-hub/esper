# Load configuration

ifndef CONFIG
include $(SITEDIR)/Configuration.mk
else
include $(SITEDIR)/Configuration.mk.$(CONFIG)
endif


# Sanity checking some required variables
ifndef SMING_HOME
$(error SMING_HOME is not set. Please configure it in Configuration.mk or set in environment.)
endif

ifndef ESP_HOME
$(error ESP_HOME is not set. Please configure it in Configuration.mk or set in environment.)
endif


# Add required modules
MODULES += $(BASEDIR)/framework/devices
MODULES += $(BASEDIR)/framework/features
MODULES += $(BASEDIR)/framework/managers
MODULES += $(BASEDIR)/framework/services
MODULES += $(BASEDIR)/framework/util
MODULES += $(BASEDIR)/framework

MODULES += $(SITEDIR)/devices/$(DEVICE)


# Ensure we have a version string
VERSION ?= ""
ifeq (VERSION, "")
$(warning VERSION is not set. Please ensure to set a version for productive builds.)
VERSION = SNAPSHOT
endif


# Load the device overrides
-include $(SITEDIR)/devices/$(DEVICE)/Device.mk


# Define config defaults
LOGGING ?= true
LOGGING_DEBUG ?= false

HEARTBEAT_ENABLED ?= false
HEARTBEAT_TOPIC ?= $(MQTT_REALM)/heartbeat

UPDATE_ENABLED ?= false
UPDATE_URL ?= $(MQTT_HOST):80/firmware
UPDATE_INTERVAL ?= 3600
UPDATE_TOPIC ?= $(MQTT_REALM)/update


# Pass options to source code
USER_CFLAGS += -DDEVICE=\"$(DEVICE)\"
USER_CFLAGS += -DVERSION=\"$(VERSION)\"

USER_CFLAGS += -DLOGGING=$(LOGGING)
USER_CFLAGS += -DLOGGING_DEBUG=$(LOGGING_DEBUG)

USER_CFLAGS += -DMQTT_HOST=\"$(MQTT_HOST)\"
USER_CFLAGS += -DMQTT_PORT=$(MQTT_PORT)
USER_CFLAGS += -DMQTT_REALM=\"$(MQTT_REALM)\"

USER_CFLAGS += -DHEARTBEAT_ENABLED=$(HEARTBEAT_ENABLED)
USER_CFLAGS += -DHEARTBEAT_TOPIC=\"$(HEARTBEAT_TOPIC)\"

USER_CFLAGS += -DUPDATE_ENABLED=$(UPDATE_ENABLED)
USER_CFLAGS += -DUPDATE_URL=\"$(UPDATE_URL)\"
USER_CFLAGS += -DUPDATE_INTERVAL=$(UPDATE_INTERVAL)
USER_CFLAGS += -DUPDATE_TOPIC=\"$(UPDATE_TOPIC)\"


# Use Release
SMING_RELEASE = 1


# Disable SPIFFS
DISABLE_SPIFFS = 1


# Use alternative bootloader
RBOOT_ENABLED = 1
RBOOT_BIG_FLASH = 0
RBOOT_TWO_ROMS = 1
RBOOT_RTC_ENABLED = 0

RBOOT_LD_0 = $(BASEDIR)/rom0.ld
RBOOT_LD_1 = $(BASEDIR)/rom1.ld


# For ESP01 - enhanced version
SPI_SIZE = 1M


# Include main makefile
include $(SMING_HOME)/Makefile-rboot.mk


# Write the manifest
all: $(FW_BASE)/version
$(FW_BASE)/version:
	echo -n "$(VERSION)" > $(FW_BASE)/version

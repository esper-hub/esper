$(info Building device '$(DEVICE)'...)

# Load configuration
ifndef CONFIG
  include $(SITEDIR)/Configuration.mk
else
  include $(SITEDIR)/Configuration.mk.$(CONFIG)
endif


# Find device module either from site or from esper
export DEVICEDIR := $(firstword $(wildcard $(SITEDIR)/devices/$(DEVICE)) $(wildcard $(BASEDIR)/devices/$(DEVICE)))


# Ensure we have a version string
VERSION ?= ""
ifeq ($(VERSION), "")
VERSION := $(shell date +%s)
ifeq ($(shell git -C $(SITEDIR) rev-parse --is-inside-work-tree), true)
VERSION := $(VERSION)-$(shell git -C $(SITEDIR) describe --always)
endif
$(warning VERSION is not set. Please ensure to set a version for productive builds. Using '$(VERSION)')
endif


# Load the device overrides
-include $(DEVICEDIR)/device.mk


# Define config defaults
LOGGING ?= true
LOGGING_DEBUG ?= false

WIFI_SSID ?= ""
WIFI_PWD ?= ""

HEARTBEAT_ENABLED ?= false
HEARTBEAT_TOPIC ?= $(MQTT_REALM)/heartbeat

UPDATE_ENABLED ?= false
UPDATE_URL ?= $(MQTT_HOST):80/firmware
UPDATE_INTERVAL ?= 3600
UPDATE_DELAY ?= 300
UPDATE_TOPIC ?= $(MQTT_REALM)/update

HTTP_ENABLED ?= false
HTTP_PORT ?= 80

MQTT_PORT ?= 1883
MQTT_URL ?= mqtt://$(MQTT_USERNAME):$(MQTT_PASSWORD)@$(MQTT_HOST):$(MQTT_PORT)


# Use Release
SMING_RELEASE = 1


# Disable SPIFFS
DISABLE_SPIFFS = 1

ENABLE_CUSTOM_LWIP = 2


# Configure bootloader for A/B partitioning
RBOOT_ENABLED = 1
RBOOT_BIG_FLASH = 0
RBOOT_TWO_ROMS = 1
RBOOT_RTC_ENABLED = 0

RBOOT_LD_0 = $(BASEDIR)/framework/rom0.ld
RBOOT_LD_1 = $(BASEDIR)/framework/rom1.ld


# For ESP01 - enhanced version
SPI_SIZE = 4M
SPI_MODE = dio
SPI_SPEED = 80


# Work-around for encoding errors in terminal during ESP boot
COM_OPTS += --raw


# Add the framework to the search path
COMPONENT_SEARCH_DIRS += $(BASEDIR)/framework


# Add esper as a dependent component for all devices
# XXX: This is a workaround as COMPONENT_DEPENDS is not setable from outside a
# component and COMPONENTS is hardly initialized in project.mk
ARDUINO_LIBRARIES := esper

# APPCODE := $(DEVICEDIR)


# Include main makefile
include $(SMING_HOME)/project.mk


# Write the manifest
all: $(FW_BASE)/version
$(FW_BASE)/version:
	echo -n "$(VERSION)" > $(FW_BASE)/version


# Copy to distribution directory
$(DISTDIR):
	mkdir -pv $(DISTDIR)

all: $(DISTDIR)/$(DEVICE).rboot
$(DISTDIR)/$(DEVICE).rboot: $(FW_BASE)/rboot.bin $(DISTDIR)
	cp -v $(FW_BASE)/rboot.bin $(DISTDIR)/$(DEVICE).rboot

all: $(DISTDIR)/$(DEVICE).rom0
$(DISTDIR)/$(DEVICE).rom0: $(FW_BASE)/rom0.bin $(DISTDIR)
	cp -v $(FW_BASE)/rom0.bin $(DISTDIR)/$(DEVICE).rom0

all: $(DISTDIR)/$(DEVICE).rom1
$(DISTDIR)/$(DEVICE).rom1: $(FW_BASE)/rom1.bin $(DISTDIR)
	cp -v $(FW_BASE)/rom1.bin $(DISTDIR)/$(DEVICE).rom1

all: $(DISTDIR)/$(DEVICE).version
$(DISTDIR)/$(DEVICE).version: $(FW_BASE)/version $(DISTDIR)
	cp -v $(FW_BASE)/version $(DISTDIR)/$(DEVICE).version

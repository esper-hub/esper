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

ifdef UPDATER_URL
MODULES += $(BASEDIR)/framework/updater
endif

MODULES += $(BASEDIR)/devices/$(DEVICE)


# Ensure we have a version string
VERSION ?= SNAPSHOT


# Pass options to source code
USER_CFLAGS += -DDEVICE=\"$(DEVICE)\"

USER_CFLAGS += -DMQTT_HOST=\"$(MQTT_HOST)\"
USER_CFLAGS += -DMQTT_PORT=$(MQTT_PORT)
USER_CFLAGS += -DMQTT_REALM=\"$(MQTT_REALM)\"

ifdef HEARTBEAT_TOPIC
USER_CFLAGS += -DHEARTBEAT_TOPIC=\"$(HEARTBEAT_TOPIC)\"
endif

ifdef UPDATER_URL
USER_CFLAGS += -DUPDATER_URL=\"$(UPDATER_URL)\"
ifdef UPDATER_INTERVAL
USER_CFLAGS += -DUPDATER_INTERVAL=$(UPDATER_INTERVAL)
endif
ifdef UPDATER_TOPIC
USER_CFLAGS += -DUPDATER_TOPIC=\"$(UPDATER_TOPIC)\"
endif
endif

USER_CFLAGS += -DVERSION=\"$(VERSION)\"


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

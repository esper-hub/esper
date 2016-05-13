include ./Makefile-user.mk

ifndef SMING_HOME
$(error SMING_HOME is not set. Please configure it in Makefile-user.mk or set in environment)
endif
ifndef ESP_HOME
$(error ESP_HOME is not set. Please configure it in Makefile-user.mk or set in environment)
endif

DISABLE_SPIFFS = 1

MODULES=src/devices src/features src/managers src/util src

ifdef MQTT_HOST
	USER_CFLAGS += -DMQTT_HOST=\"$(MQTT_HOST)\"
endif

ifdef MQTT_PORT
	USER_CFLAGS += -DMQTT_PORT=$(MQTT_PORT)
endif

ifdef REALM
        USER_CFLAGS += -DREALM=\"$(REALM)\"
endif

# Include main Sming Makefile
ifeq ($(RBOOT_ENABLED), 1)
include $(SMING_HOME)/Makefile-rboot.mk
else
include $(SMING_HOME)/Makefile-project.mk
endif

# Load configuration
ifndef CONFIG
include $(SITEDIR)/Configuration.mk
else
include $(SITEDIR)/Configuration.mk.$(CONFIG)
endif

# This is a library which contains the init
COMPONENT_LIBNAME = "esper"

# Add source code
COMPONENT_SRCDIRS += $(COMPONENT_PATH)/features
COMPONENT_SRCDIRS += $(COMPONENT_PATH)/managers
COMPONENT_SRCDIRS += $(COMPONENT_PATH)/services
COMPONENT_SRCDIRS += $(COMPONENT_PATH)/util
COMPONENT_SRCDIRS += $(COMPONENT_PATH)

# Add the framework itself as include to make it usable in the device code
COMPONENT_INCDIRS += $(COMPONENT_PATH)/framework

# Ensure rebuild for each device
COMPONENT_VARS += DEVICE VERSION
COMPONENT_VARS += WIFI_SSID WIFI_PWD
COMPONENT_VARS += MQTT_HOST MQTT_PORT MQTT_USERNAME MQTT_PASSWORD MQTT_REALM
COMPONENT_VARS += HTTP_ENABLED HTTP_PORT
COMPONENT_VARS += HEARTBEAT_ENABLED HEARTBEAT_TOPIC
COMPONENT_VARS += UPDATE_ENABLED UPDATE_URL UPDATE_INTERVAL

# Export config
GLOBAL_CFLAGS += -DDEVICE=\"$(DEVICE)\"
GLOBAL_CFLAGS += -DVERSION=\"$(VERSION)\"

GLOBAL_CFLAGS += -DLOGGING=$(LOGGING)
GLOBAL_CFLAGS += -DLOGGING_DEBUG=$(LOGGING_DEBUG)

GLOBAL_CFLAGS += -DWIFI_SSID=\"$(WIFI_SSID)\"
GLOBAL_CFLAGS += -DWIFI_PWD=\"$(WIFI_PWD)\"

GLOBAL_CFLAGS += -DMQTT_URL=\"$(MQTT_URL)\"
GLOBAL_CFLAGS += -DMQTT_REALM=\"$(MQTT_REALM)\"

GLOBAL_CFLAGS += -DHEARTBEAT_ENABLED=$(HEARTBEAT_ENABLED)
GLOBAL_CFLAGS += -DHEARTBEAT_TOPIC=\"$(HEARTBEAT_TOPIC)\"

GLOBAL_CFLAGS += -DUPDATE_ENABLED=$(UPDATE_ENABLED)
GLOBAL_CFLAGS += -DUPDATE_URL=\"$(UPDATE_URL)\"
GLOBAL_CFLAGS += -DUPDATE_INTERVAL=$(UPDATE_INTERVAL)
GLOBAL_CFLAGS += -DUPDATE_DELAY=$(UPDATE_DELAY)
GLOBAL_CFLAGS += -DUPDATE_TOPIC=\"$(UPDATE_TOPIC)\"

GLOBAL_CFLAGS += -DHTTP_ENABLED=$(HTTP_ENABLED)
GLOBAL_CFLAGS += -DHTTP_PORT=$(HTTP_PORT)

# Of course, we build against sming
COMPONENT_DEPENDS += Sming

# Add JSON support
COMPONENT_DEPENDS += ArduinoJson6

# Reexport some application variables
COMPONENT_CXXFLAGS += -DCOM_SPEED_SERIAL=$(COM_SPEED_SERIAL)

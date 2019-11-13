ifndef SITE
$(error SITE is not set. Please Specify a site directory.)
else
export SITEDIR := $(abspath $(SITE))
endif


export BASEDIR := $(CURDIR)
export DISTDIR := $(SITEDIR)/dist

# Find devices
DEVICES := $(sort $(notdir $(wildcard $(SITEDIR)/devices/*) $(wildcard $(BASEDIR)/devices/*)))

devicedir = $(firstword $(wildcard $(SITEDIR)/devices/$(1)) $(wildcard $(BASEDIR)/devices/$(1)))


.PHONY: all
all: $(DEVICES)


.PHONY: $(addsuffix /clean,$(DEVICES))
$(addsuffix /clean,$(DEVICES)): %/clean:
	$(MAKE) -C $(call devicedir,$(*)) -f $(BASEDIR)/framework/device.mk DEVICE=$(*) clean


.PHONY: $(addsuffix /build,$(DEVICES))
$(addsuffix /build,$(DEVICES)): %/build: %/clean
	$(MAKE) -C $(call devicedir,$(*)) -f $(BASEDIR)/framework/device.mk DEVICE=$(*) all


.PHONY: $(addsuffix /flash,$(DEVICES))
$(addsuffix /flash,$(DEVICES)): %/flash: %/build
	$(MAKE) -C $(call devicedir,$(*)) -f $(BASEDIR)/framework/device.mk DEVICE=$(*) flashinit flash


.PHONY: $(DEVICES)
$(DEVICES): %: %/build


.PHONY: clean
clean:
	rm -f -r $(DISTDIR)

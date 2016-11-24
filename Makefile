export BASEDIR := $(CURDIR)

BUILDDIR = $(BASEDIR)/build

# Find devices
DEVICES := $(sort $(notdir $(wildcard $(CURDIR)/devices/*)))


all: $(DEVICES)


$(DEVICES):
	mkdir -pv $(BUILDDIR)/$@
	$(MAKE) -C $(BUILDDIR)/$@ -f $(CURDIR)/Device.mk DEVICE=$@


clean:
	rm -f -r $(BUILDDIR)

.PHONY: all $(DEVICES) clean


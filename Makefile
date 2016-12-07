export BASEDIR := $(CURDIR)


BUILDDIR = $(BASEDIR)/build
DISTDIR = $(BASEDIR)/dist


# Find devices
DEVICES := $(sort $(notdir $(wildcard $(CURDIR)/devices/*)))


.PHONY: all
all: $(DEVICES)


.PHONY: $(addsuffix /build,$(DEVICES))
$(addsuffix /build,$(DEVICES)): %/build:
	mkdir -p $(BUILDDIR)/$*
	$(MAKE) -C $(BUILDDIR)/$* -f $(BASEDIR)/Device.mk DEVICE=$*


.PHONY: $(addsuffix /flash,$(DEVICES))
$(addsuffix /flash,$(DEVICES)): %/flash: %/build
	$(MAKE) -C $(BUILDDIR)/$* -f $(BASEDIR)/Device.mk DEVICE=$* flash


.PHONY: $(addsuffix /dist,$(DEVICES))
$(addsuffix /dist,$(DEVICES)): %/dist: %/build
	mkdir -p $(DISTDIR)
	cp $(BUILDDIR)/$*/out/firmware/rboot.bin $(DISTDIR)/$*.rboot
	cp $(BUILDDIR)/$*/out/firmware/rom0.bin $(DISTDIR)/$*.rom0
	cp $(BUILDDIR)/$*/out/firmware/rom1.bin $(DISTDIR)/$*.rom1
	cp $(BUILDDIR)/$*/out/firmware/version $(DISTDIR)/$*.version


.PHONY: $(DEVICES)
$(DEVICES): %: %/build %/dist


.PHONY: clean
clean:
	rm -f -r $(BUILDDIR) $(DISTDIR)

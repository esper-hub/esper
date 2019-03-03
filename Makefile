export BASEDIR := $(CURDIR)


ifndef SITE
$(error SITE is not set. Please Specify a site directory.)
else
export SITEDIR := $(abspath $(SITE))
endif


export BUILDDIR = $(BASEDIR)/build
export DISTDIR = $(SITEDIR)/dist


# Find devices
DEVICES := $(sort $(notdir $(wildcard $(SITEDIR)/devices/*) $(wildcard $(BASEDIR)/devices/*)))


.PHONY: all
all: $(DEVICES)


.PHONY: $(addsuffix /clean,$(DEVICES))
$(addsuffix /clean,$(DEVICES)): %/clean:
	mkdir -p $(BUILDDIR)/$*
	$(MAKE) -C $(BUILDDIR)/$* -f $(BASEDIR)/Device.mk DEVICE=$* clean


.PHONY: $(addsuffix /build,$(DEVICES))
$(addsuffix /build,$(DEVICES)): %/build:
	mkdir -p $(BUILDDIR)/$*
	$(MAKE) -C $(BUILDDIR)/$* -f $(BASEDIR)/Device.mk DEVICE=$*


.PHONY: $(addsuffix /erase,$(DEVICES))
$(addsuffix /erase,$(DEVICES)): %/erase: %/build
	$(MAKE) -C $(BUILDDIR)/$* -f $(BASEDIR)/Device.mk DEVICE=$* flashinit


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

export BASEDIR := $(CURDIR)

BUILDDIR = $(BASEDIR)/build
DISTDIR = $(BASEDIR)/dist

# Find devices
DEVICES := $(sort $(notdir $(wildcard $(CURDIR)/devices/*)))


all: $(DEVICES)


$(addprefix $(BUILDDIR)/,$(DEVICES)):
	mkdir -pv $@
	$(MAKE) -C $@ -f $(BASEDIR)/Device.mk DEVICE=$(notdir $@)


$(DEVICES): %: $(BUILDDIR)/% $(DISTDIR)/%.rboot $(DISTDIR)/%.rom0 $(DISTDIR)/%.rom1


$(DISTDIR)/%.rboot: $(BUILDDIR)/% $(DISTDIR)
	cp -v $</out/firmware/rboot.bin $@


$(DISTDIR)/%.rom0: $(BUILDDIR)/% $(DISTDIR)
	cp -v $</out/firmware/rom0.bin $@


$(DISTDIR)/%.rom1: $(BUILDDIR)/% $(DISTDIR)
	cp -v $</out/firmware/rom1.bin $@


$(DISTDIR):
	mkdir -pv $@


clean:
	rm -f -r $(BUILDDIR) $(DISTDIR)

.PHONY: all $(addprefix $(BUILDDIR)/,$(DEVICES)) clean


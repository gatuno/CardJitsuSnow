# Automake file for Card-Jitsu Snow

gamedatadir = $(pkgdatadir)/data

bin_PROGRAMS = cjsnow
cjsnow_SOURCES = snow.c path.c path.h \
	uitimer.c uitimer.h \
	netplay.h netplay.c \
	water_ninja.c water_ninja.h \
	fire_ninja.c fire_ninja.h \
	snow_ninja.c snow_ninja.h \
	enemy.c enemy.h
servidor_SOURCES = server.c netplay.c netplay.h snow.h \
	server_ninja.c server_ninja.h

# El servidor sólo funciona en Linux
if LINUX
bin_PROGRAMS += servidor
endif

EXTRA_DIST = snow.rc

if MINGW32
snow_ico.o: snow.rc ../data/snow.ico
	$(WINDRES) $(srcdir)/snow.rc -O coff -o snow_ico.o

mingw_ldadd = snow_ico.o

else # NOT MINGW
mingw_ldadd =
endif

cjsnow_CPPFLAGS = -DGAMEDATA_DIR=\"$(gamedatadir)/\" -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
cjsnow_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(SDL_mixer_CFLAGS) $(SDL_ttf_CFLAGS) $(AM_CFLAGS)
servidor_CPPFLAGS = -DGAMEDATA_DIR=\"$(gamedatadir)/\" -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
servidor_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(SDL_mixer_CFLAGS) $(SDL_ttf_CFLAGS) $(AM_CFLAGS)
if MACOSX
# En MAC OS X, hay que ligar/compilar contra los frameworks
cjsnow_LDADD = $(SDL_LIBS)
else
cjsnow_LDADD = -lm $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_mixer_LIBS) $(SDL_ttf_LIBS) $(mingw_ldadd)
servidor_LDADD = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_mixer_LIBS) $(SDL_ttf_LIBS) $(mingw_ldadd)
endif
LDADD = $(LIBINTL)

#------------------ Packaging rules for Mac OSX ------------------------

bundle_root = $(top_builddir)/etc/macfiles
bundle_name = $(bundle_root)/Card-JitsuSnow.app
# Special target to copy all needed game files to PaddlePuffle.app/Contents/Resources.
# For Mac OS X bundle.
.PHONY: bundle
bundle:
	-rm -rf $(bundle_root)
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources/data/
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	cp $(top_builddir)/etc/Info.plist $(bundle_name)/Contents/
	cp $(top_builddir)/etc/snow.icns $(bundle_name)/Contents/Resources/
	mkdir -p $(bundle_name)/Contents/Resources/data/images
	cp $(top_builddir)/data/images/* $(bundle_name)/Contents/Resources/data/images
	mkdir -p $(bundle_name)/Contents/Resources/data/music
	cp $(top_builddir)/data/music/* $(bundle_name)/Contents/Resources/data/music
	mkdir -p $(bundle_name)/Contents/Resources/data/sounds
	cp $(top_builddir)/data/sounds/* $(bundle_name)/Contents/Resources/data/sounds
	cp $(top_builddir)/data/*.ttf $(bundle_name)/Contents/Resources/data
	for cat in $(ALL_LINGUAS); do \
	  dir=$(bundle_name)/Contents/Resources/locale/$$cat/LC_MESSAGES; \
	  mkdir -p $$dir; \
	  echo "installing $$cat as $$dir/card_jitsu_snow.mo"; \
	  cp $(top_builddir)/po/$$cat.gmo $$dir/card_jitsu_snow.mo; \
	done

# Special target to copy the binary to PaddlePuffle.app/Contents/MacOS.
.PHONY: bundle-fw
bundle-fw: bundle
	cp cjsnow $(bundle_name)/Contents/MacOS/card_jitsu_snow
	mkdir  $(bundle_name)/Contents/Frameworks
	for i in SDL SDL_mixer SDL_image SDL_ttf ; do \
	  cp -a /Library/Frameworks/$${i}.framework $(bundle_name)/Contents/Frameworks/; \
	done
	strip $(bundle_name)/Contents/MacOS/cjsnow

# Special target to make a disk image of the bundled app
.PHONY: bundle-dmg
bundle-dmg: bundle-fw
	ln -s /Applications $(bundle_root)/Applications
	hdiutil create -ov -srcfolder $(bundle_root) -volname Card-JitsuSnow -imagekey zlib-level=6 $(top_builddir)/etc/card_jitsu_snow.dmg


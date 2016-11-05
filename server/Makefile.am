# Automake file for Card-Jitsu Snow Server

servidor_SOURCES = server.c netplay.c netplay.h snow.h \
	server_ninja.c server_ninja.h \
	server_timer.c server_timer.h \
	server_ia.c server_ia.h
stand_alone_SOURCES = server.c netplay.c netplay.h snow.h \
	server_ninja.c server_ninja.h \
	server_timer.c server_timer.h \
	server_ia.c server_ia.h

# El servidor sólo funciona en Linux
if LINUX
bin_PROGRAMS = servidor stand_alone
else
bin_PROGRAMS = 
endif

servidor_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
servidor_CFLAGS = $(AM_CFLAGS)
stand_alone_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -DSTAND_ALONE=1 $(AM_CPPFLAGS)
stand_alone_CFLAGS = $(AM_CFLAGS)

servidor_LDADD = -lm
stand_alone_LDADD = -lm
LDADD = $(LIBINTL)


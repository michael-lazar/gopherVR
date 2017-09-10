# Makefile for any machine
# - based on my newer makefile and the GREATE
# modifications by Mike Battersby
###############################################################################
#
# CONFIGURATION OPTIONS
#

# Choose your machine type. Possible types as of this version (4.11)
# are
#  hpalib, hplow, hp3, dec, solaris, sparc, linux, freebsd, sgi, 
#  openbsd, soundblaster, aix, next.
# gmake needed to handle this file in FreeBSD
MACHINE = sparc
VERSION = 4.27

# The name of your C compiler. For most machines this will be
# either 'cc' or 'gcc'.
CC = gcc

OPTS=-c

# User interface type. Right now, this is Unix as there are no alternatives.
UI = Unix/./

PREFIX=/usr/local
# Destination directory for tracker binaries and manpage.
#
# If you don't wish to use the 'make install' and 'make install.man'
# commands, you can ignore these.
BIN_DIR = $(PREFIX)/bin
MAN_DIR = $(PREFIX)/man

# Where to put the compression methods description
COMPRESSION_FILE=$(PREFIX)/lib/compression_methods

# How to install the binaries and manpage. If you have a unix system,
# use the GNU install program if you have it, otherwise try 'cp'. For
# non-unix systems, make this the command for copying a file with the
# following syntax
#  <program_name> <source_name> <destination_name>
#
# If you don't wish to use the 'make install' and 'make install.man'
# commands, you can ignore this.
INSTALL = install

# Permissions and user/group id's to install with. Non-unix users
# should comment these out (alternatively, you can use these to
# provide arguments for the above $(INSTALL) program). If you want
# to install tracker setuid root (which will allow it to renice
# itself when non-root users run it), change the "-m 755" below to
# "-m 4755". If you do so, you must be root to run make install.
#
# If you don't wish to use the 'make install' and 'make install.man'
# commands, you can ignore these.
INST_BIN_OPT = -c -s -m 755 -o root -g root
INST_MAN_OPT = -c -m 644 -o root -g root
###############################################################################
#
# C compiler flags and libraries for each machine.
# Unless you are porting this to a new architecture, you shouldn't
# need to change these (on the other hand, if you understand what you
# are doing, go ahead!).
#

CFLAGS_next = -O3 -fomit-frame-pointer -funroll-loops -ffast-math -Wall
LIBS_next   =
AUDIODIR_next = NeXT/
CONFIG_next = NeXT/

CFLAGS_hpalib = +O3 +OS +ESlit
LIBS_hpalib   = -lAt -lAlib -lm
AUDIODIR_hpalib=Hpux/
AUDIONAME_hpalib=alib_
CONFIG_hpalib = Hpux/

CFLAGS_hplow = -Ae +O3 -Wl, -a,archive -s
LIBS_hplow=-lM
AUDIODIR_hplow=Hpux/
AUDIONAME_hplow=low_
CONFIG_hplow = Hpux/

# yet another port to hp! This one might be the best yet.
CFLAGS_hp3 = +O3 +OS +ESlit
LIBS_hp3   = -lAt -lAlib -lm
AUDIODIR_hp3=Hpux/
AUDIONAME_hp3=3_
CONFIG_hp3 = Hpux/

CFLAGS_solaris = -O
LIBS_solaris   = -lm
AUDIODIR_solaris=Sparc/ 
CONFIG_solaris = Sparc/solaris_


#CFLAGS_sparc = -g
#CFLAGS_sparc = -O4 -Bstatic
#CFLAGS_sparc = -g -D__USE_FIXED_PROTOTYPES__
CFLAGS_sparc = -O2 -finline-functions -funroll-loops -Wall \
-D__USE_FIXED_PROTOTYPES__ -static
LIBS_sparc   = -lm
AUDIODIR_sparc=Sparc/
CONFIG_sparc = Sparc/

CFLAGS_dec = -O 
LIBS_dec   = -lm -lAF
AUDIODIR_dec=AF/
CONFIG_dec = AF/dec_

CFLAGS_sgi = -O 
LIBS_sgi   = -laudio -lm
AUDIODIR_sgi=Sgi/
CONFIG_sgi = Sgi/

CFLAGS_sgidummy = -O
LIBS_sgidummy   = -laudio -lm
AUDIODIR_sgidummy = dummy/
CONFIG_sgidummy = dummy/

CFLAGS_soundblaster = -O
LIBS_soundblaster   = -lm
AUDIODIR_soundblaster=Soundblaster/
CONFIG_soundblaster = Soundblaster/

CFLAGS_linux = -O2 -funroll-loops
LIBS_linux   = -lm
AUDIODIR_linux=PCux/
CONFIG_linux = PCux/linux_

CFLAGS_freebsd = -O2 -funroll-loops -finline-functions
LIBS_freebsd = -lm
AUDIODIR_freebsd=PCux/
CONFIG_freebsd = PCux/freebsd_

CFLAGS_openbsd = -O2 -pipe -funroll-loops -finline-functions
LIBS_openbsd = -lm -lossaudio
AUDIODIR_openbsd=PCux/
CONFIG_openbsd = PCux/openbsd_

CFLAGS_aix = -O
LIBS_aix = -lm
AUDIODIR_aix = Aix/
CONFIG_aix = Aix/

CFLAGS_NAS = -O -I/usr/X11R6
LIBS_NAS = -lm -L/usr/X11R6/lib -laudio -lX11
AUDIODIR_NAS = NAS/
CONFIG_NAS = NAS/

CFLAGS_dummy = -O -I/usr/X11R6
LIBS_dummy = -lm -L/usr/X11R6/lib -lX11
AUDIODIR_dummy = dummy/
CONFIG_dummy = dummy/

CFLAGS_osxu = -O3 -I/usr/X11R6 -force_cpusubtype_ALL -mmacosx-version-min=10.4 -arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk
LIBS_osxu = -lm -L/usr/X11R6/lib -lX11
AUDIODIR_osxu = dummy/
CONFIG_osxu = dummy/

CFLAGS_osx = -O3 -I/usr/X11R6 -mmacosx-version-min=10.4 -arch i386
LIBS_osx = -lm -L/usr/X11R6/lib -lX11
AUDIODIR_osx = dummy/
CONFIG_osx = dummy/

CFLAGS_SVR4 = -O -I/usr/X/include
LIBS_SVR4 = -L/usr/X/lib $(LIBS_NAS) -lsocket -lnsl -lc /usr/ucblib/libucb.a
AUDIODIR_SVR4 = $(AUDIODIR_NAS)
CONFIG_SVR4 = $(CONFIG_NAS)

CFLAGS = $(CFLAGS_${MACHINE})
COPTS = $(OPTS) $(CFLAGS)
LIBS = $(LIBS_${MACHINE})
CONFIG = Arch/$(CONFIG_${MACHINE})
AUDIODIR = Arch/$(AUDIODIR_${MACHINE})
AUDIONAME = $(AUDIONAME_${MACHINE})
AUDIO = $(AUDIODIR)$(AUDIONAME)

OBJ_LIBTRACKER = $(AUDIO)audio.o st_read.o commands.o \
resample.o automaton.o st_play.o getopt.o open.o tools.o \
dump_song.o setup_audio.o notes.o display.o empty.o \
$(UI)ui.o prefs.o tags.o autoinit.o color.o version.o

OBJ_TRACKER = main.o $(OBJ_LIBTRACKER)

OBJ_ANALYZER = analyzer.o st_read.o open.o dump_song.o tools.o notes.o \
prefs.o autoinit.o ${UI}ui.o display.o empty.o color.o version.o

OBJ_SPLITMOD = split.o tools.o st_read.o dump_song.o open.o notes.o \
display.o prefs.o autoinit.o $(UI)ui.o empty.o color.o version.o

all:	version.c config.h tracker randomize tcheck splitmod ../lib/libtracker.a

config.h: $(CONFIG)config.h Makefile
	cp $(CONFIG)config.h config.h

version.c: Makefile
	echo >version.c char \*VERSION=\"${VERSION}\"\;

install: 
	$(INSTALL) $(INST_BIN_OPT) tracker $(BIN_DIR)
	$(INSTALL) $(INST_BIN_OPT) randomize $(BIN_DIR)
	$(INSTALL) $(INST_BIN_OPT) tcheck $(BIN_DIR)
	$(INSTALL) $(INST_BIN_OPT) splitmod $(BIN_DIR)
	[ -f ${COMPRESSION_FILE} ] || \
		$(INSTALL) $(INST_MAN_OPT) compression_methods ${COMPRESSION_FILE}

install.man:
	$(INSTALL) $(INST_MAN_OPT) man/tracker.1 $(MAN_DIR)/man1/tracker.1



devel: 
	-rm -f $(BIN_DIR)/dtracker #$(BIN_DIR)/splitmod
	cp tracker $(BIN_DIR)/dtracker
#-	cp splitmod $(BIN_DIR)/splitmod

tracker: ${OBJ_TRACKER}
	${CC} -o tracker ${CFLAGS} ${OBJ_TRACKER} ${LIBS}

../lib/libtracker.a: ${OBJ_LIBTRACKER} libtracker.o
	rm -f ../lib/libtracker.a
	ar rcv ../lib/libtracker.a ${OBJ_LIBTRACKER} libtracker.o
	ranlib ../lib/libtracker.a || true


dtracker: ${OBJ_TRACKER}
	${CC} -o dtracker ${CFLAGS} ${OBJ_TRACKER} ${LIBS}

tcheck: ${OBJ_ANALYZER}
	$(CC) -o tcheck ${CFLAGS} ${OBJ_ANALYZER} ${LIBS}

randomize: randomize.c
	$(CC) -o randomize ${CFLAGS} randomize.c 

splitmod: ${OBJ_SPLITMOD}
	$(CC) -o splitmod ${CFLAGS} ${OBJ_SPLITMOD} ${LIBS}

main.o: main.c song.h 
	$(CC) ${COPTS} main.c

tools.o: main.c 
	$(CC) ${COPTS} tools.c

$(AUDIO)audio.o: $(AUDIO)audio.c Arch/common.c
	cd $(AUDIODIR); $(CC) -I../.. ${COPTS} $(AUDIONAME)audio.c

$(UI)ui.o: $(UI)ui.c
	cd $(UI); $(CC) -I.. ${COPTS} ui.c

split.o: split.c
	$(CC) ${COPTS} split.c

open.o: open.c Makefile
	$(CC) ${COPTS} -DCOMPRESSION_FILE=\"${COMPRESSION_FILE}\" open.c

resample.o: resample.c
	$(CC) ${COPTS} resample.c

automaton.o: automaton.c song.h channel.h
	$(CC) ${COPTS} automaton.c

getopt.o: getopt.c getlongopt.h
	$(CC) ${COPTS} getopt.c

player.o: player.c song.h channel.h
	$(CC) ${COPTS} player.c

st_read.o: st_read.c song.h
	$(CC) ${COPTS} st_read.c

commands.o: commands.c channel.h song.h
	$(CC) ${COPTS} commands.c

dump_song.o: dump_song.c
	$(CC) $(COPTS) dump_song.c
analyzer.o: analyzer.c
	$(CC) $(COPTS) analyzer.c
setup_audio.o: setup_audio.c
	$(CC) $(COPTS) setup_audio.c
notes.o: notes.c
	$(CC) $(COPTS) notes.c

display.o: display.c
	$(CC) $(COPTS) display.c

libtracker.o: libtracker.c
	$(CC) $(COPTS) libtracker.c


clean:
	-rm -f $(OBJ_TRACKER) $(OBJ_ANALYZER) $(OBJ_SPLITMOD) randomize.o libtracker.o 
	-rm -f tracker randomize splitmod tcheck core 
	-rm -f tracker*.tar tracker\*.tar.Z tracker\*.lzh
	-rm -f config.h version.c
	find . -name \*.bak -exec rm {} \;

export:
	-rm tracker*.tar.Z tracker*.lzh tracker*.lha
	cd ..; tar -cvof tracker/tracker-$(VERSION).tar -I tracker/arc.list
	mv tracker-$(VERSION).tar /tmp
	cd /tmp; tar xvf tracker-$(VERSION).tar
	cd /tmp; lha a tracker-$(VERSION).lzh tracker
	compress /tmp/tracker-$(VERSION).tar
	cp /tmp/tracker-$(VERSION).lzh .
	cp /tmp/tracker-$(VERSION).tar.Z .
	rm -rf /tmp/tracker*;


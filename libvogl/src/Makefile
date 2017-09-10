#
# Make the vogl library.
#
# Below are our defaults for compliers etc. These are set in the
# main Makefile.
#

CC = gcc

MCFLAGS = #-g -fsingle

LIB = libvogl.a 

HDRS =	vogl.h

CSRC =	arcs.c attr.c buffer.c clip.c curves.c draw.c drivers.c \
	viewing.c viewp.c mapping.c matrix.c move.c objects.c patches.c \
	points.c polygons.c rect.c tensor.c text.c newtokens.c trans.c \
	scale.c getgp.c valloc.c yobbarays.c pref.c vcalls.c \
	queue.c lines.c bsp.c minv4.c geometry.c shading.c eyemotion.c \
	render_dir.c render_document.c render_kiosk.c render_search.c \
	render_telnet.c render_sound.c shading_d.c

COBJS =	arcs.o attr.o buffer.o clip.o curves.o draw.o drivers.o \
	viewing.o viewp.o mapping.o matrix.o move.o objects.o patches.o \
	points.o polygons.o rect.o tensor.o text.o newtokens.o trans.o \
	scale.o getgp.o valloc.o yobbarays.o pref.o vcalls.o \
	queue.o lines.o bsp.o minv4.o geometry.o shading.o eyemotion.o \
	render_dir.o render_document.o render_kiosk.o render_search.o \
	render_telnet.o render_sound.o shading_d.o


CFLAGS= $(DEVICES) $(MCFLAGS) 


../../lib/$(LIB): $(DOBJS) $(COBJS) vogl.h
	rm -f $@
	ar rcv $@ $(DOBJS) $(COBJS) 
	ranlib $@ || true

clean:
	rm -f $(COBJS) core *~ a.out
	cd ../drivers; $(MAKE) $(MFLAGS) clean

clobber:
	rm -f ../../lib/$(LIB) $(COBJS) $(FOBJS) core *.lint
	cd sunfort; make clean

$(COBJS): vogl.h

$(DOBJS): vogl.h ../drivers/X11.c
	cd ../drivers; $(MAKE) $(MFLAGS) -f Makefile \
			"CC=$(CC)" \
			MCFLAGS="$(MCFLAGS)" \
			DOBJS="$(DOBJS)"


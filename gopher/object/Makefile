srcdir = .
top_srcdir = ..


include ../Makefile.config


CCFLAGS	= $(OPT) -force_cpusubtype_ALL -mmacosx-version-min=10.4 -arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk $(INTLOPTS) $(DEBUGGING) $(WAISTYPE) $(SOCKSFLAGS) -I.. -I$(srcdir)  -I$(top_srcdir) 

OBJS	= STRstring.o GDgopherdir.o GSgopherobj.o \
	  DAarray.o   VIews.o       BLblock.o \
	  Sockets.o   util.o        compatible.o \
	  url.o       Debug.o       Regex.o \
	  fileio.o    Locale.o      RBTree.o


TARGET	= libgopher.a libgopher.so.0.1

all: shared $(TARGET)

#
# Make shared library directories
#
shared:
	@-if [ -f /lib/ld.so -a "$(SHAREDLIB)" = "sun" ]; \
	then \
		mkdir shared-sun; \
	fi
	touch shared


.c.o: $(OBJS)
	gcc $(CCFLAGS) $(PIC) -c $<
	@-if [ -d shared-sun ]; then \
		echo "Building shared version of $@"; \
		gcc $(CCFLAGS) -fPIC -o shared-sun/$@ -c $<; \
	fi

#
# Stuff for shared libraries
#

libgopher.so.0.1: $(OBJS)
	@-if [ -d shared-sun ]; then \
		echo "Building shared library $@"; \
		cd shared-sun; \
		rm -f $@; \
		ld -o $@ -assert pure-text $(OBJS);\
		mv $@ ..; \
	fi

libgopher.sa.0.1: $(OBJS)
	;


libgopher.a: $(OBJS)
	rm -f libgopher.a
	ar rcv libgopher.a $(OBJS)
	ranlib libgopher.a || true

clean:
	-rm -f $(TARGET) $(OBJS) *.out *~ core TAGS \
	       *.sa.* shared
	-rm -rf shared-sun

GSgopherobj.o:	$(srcdir)/GSgopherobj.h


Locale.o: $(srcdir)/Locale.h $(srcdir)/Locale.c
	gcc $(CCFLAGS) $(PIC) -DLOCALEDIR=\"$(CLIENTLIB)/gophernls\" -c $(srcdir)/Locale.c
	@-if [ -d shared-sun ]; then \
		echo "Building shared version of $@"; \
		gcc $(CCFLAGS) -DLOCALEDIR=\"$(CLIENTLIB)/gophernls\" \
			-fPIC -o shared-sun/$@ -c $(srcdir)/Locale.c; \
	fi

#
#install shared libraries if need be..
#
install:
	@-if [ -f *.so.* ]; then \
	  echo "Installing Sun shared gopher library" ;\
	  /usr/bin/install -c libgopher.so.0.1 $(CLIENTLIB); \
	  /usr/bin/install -c libgopher.a $(CLIENTLIB); \
	fi


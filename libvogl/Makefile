#
# Makefile for vogl (On non SGI-GL machines, if you are on an SGI and
# plan to use the real GL (as opposed to X11 on an SGI)
#

SHELL = /bin/sh

# 
#  The devices you wish to have compiled into the library and the
#  location of the object files for each device relative to the 
#  src directory. For each device defined there should be a 
#  corresponding ../drivers/file.o in the DOBJS line.
#
#  Possible DEVICES and their object files are:
#		-DPOSTSCRIPT	../drivers/ps.o


#		-DX11		../drivers/X11.o
#

DEVICES = -DX11 -DUNIX
DOBJS = ../drivers/X11.o

# Where the fonts a going to live (For making the Hershey library)
# (Remember /tmp usually gets cleared from time to time)
#
FONTLIB = /usr/local/lib/hershey/

#
#  For BSD type machines we need to use ranlib
#
RANLIB = ranlib


######################################################################
#
#Different C compilers
GCC 		= gcc -ansi -pedantic -Wall -Wstrict-prototypes
OSXGCC		= $(GCC) -mmacosx-version-min=10.4 -arch i386
UOSXGCC		= $(GCC) -force_cpusubtype_ALL -mmacosx-version-min=10.4 -arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk
SUNPRO_CC	= acc
AIXCC		= xlc
IRIX5CC		= cc -fullwarn 
SOL2CC		= cc
OSF2CC		= cc 
#
# Set any Special floating point options here...
#
SUNPRO_FLOAT		= -cg89 -fnonstd -libmil 
SUN_GCC_FLOAT 		= -ffast-math -mv8
SGI_GCC_FLOAT		= -ffast-math
GCC_FLOAT		= -ffast-math
LINUX_FLOAT 		= -ffast-math
OPENBSD_FLOAT 		= -ffast-math
NEXT_FLOAT 		= -ffast-math
MACOSX_FLOAT 		= -ffast-math
AIX4_FLOAT		= -qfloat=fold:hssngl:maf -qignerrno 
AIX3_FLOAT		= -qfloat=fold:hssngl:maf 
IRIX5_FLOAT		= -float
SOL2_FLOAT		= -fnonstd -fsingle -xcg92 -xlibmil 
OSF2_FLOAT		= 

#
# Specific Optimization flags
#
SUNPRO_OPT		= -g -dalign -strconst -bsdmalloc -I/usr/local/include
SUN_GCC_OPT		= -O3 -finline-functions -fexpensive-optimizations 
SGI_GCC_OPT		=  -finline-functions -fexpensive-optimizations -g
NEXT_OPT		= -O3 -finline-functions -fexpensive-optimizations -m68040
LINUX_OPT		= -g -O3 -fomit-frame-pointer -finline-functions -fexpensive-optimizations -DNO_MULTIBUF
#LINUX_OPT		= -g  -finline-functions -fexpensive-optimizations -m486 -DNO_MULTIBUF
OPENBSD_OPT		= -O2 -pipe -g -fomit-frame-pointer -finline-functions -fexpensive-optimizations -I/usr/local/include -I/usr/X11R6/include
SOL2X86_OPT		= -O3 -fomit-frame-pointer -finline-functions -fexpensive-optimizations -I/usr/openwin/include -DSYS5
AIX4_OPT		= -g -O3 -qnostrict -Q -qunroll -qansialias -qro
AIX3_OPT		= -O3 -qnostrict -Q -qansialias -qro
IRIX5_OPT		= -O2 -g3
SOL2_OPT		= -dalign -xO4 -xstrconst -I/usr/openwin/include -DSYS5
OSF2_OPT		= -g3  -O2 -check 
MACOSX_OPT		= -g -O3 -fomit-frame-pointer -finline-functions -fexpensive-optimizations -DNO_MULTIBUF -I/opt/X11/include
######################################################################


#
# The name of the library to install and where to put it.
#
LIB = libvogl.a
DEST = /usr/local/lib


#
# Any other local libraries that are needed go here
#

# X11
LLIBS = -lX11
# SGI
#LLIBS = -lgl_s

LIBM = -lm

LIBS = $(LLIBS) -lm
MAKEDEFS = DEVICES="$(DEVICES)" DOBJS="$(DOBJS)"

all:
	@echo "Type make <platform>"
	@echo " "
	@echo "where platform is: "
	@echo " "
	@echo " sun-gcc      Sun using gcc"
	@echo " sunpro       Sun using unbundled C compiler"
	@echo " sol2sparc    Sparc Solaris 2 using Sun unbundled compiler"
	@echo " sol2x86-gcc  x86 Solaris 2 with gcc"
	@echo " aix3         AIX version 3 with bundled compiler"
	@echo " aix4         AIX version 4 with unbundled compiler"
	@echo " irix5        Irix 5"
	@echo " linux        Linux w/ gcc"
	@echo " openbsd      OpenBSD with gcc or clang"
	@echo " macosx       i386 Mac OS X (10.4+ supported)"
	@echo " umacosx      ppc/i386 Mac OS X (10.4+ supported)"
 
######################################################################
debug-sun:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" $(MAKEDEFS) \
			MCFLAGS="$(SUN_GCC_FLOAT) $(SUN_GCC_OPT) -g -pg"

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(SUN_GCC_FLOAT) $(SUN_GCC_OPT) -g -pg"

######################################################################
sun-gcc:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" $(MAKEDEFS) \
			MCFLAGS="$(SUN_GCC_FLOAT) $(SUN_GCC_OPT)" \

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(SUN_GCC_FLOAT) $(SUN_GCC_OPT)"

######################################################################
linux:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
	CC="$(GCC)" $(MAKEDEFS) \
	MCFLAGS="$(LINUX_FLOAT) $(LINUX_OPT)" 

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(LINUX_FLOAT) $(LINUX_OPT)"

######################################################################
openbsd:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
	CC="$(CC)" $(MAKEDEFS) \
	MCFLAGS="$(OPENBSD_FLOAT) $(OPENBSD_OPT)" 

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(CC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(OPENBSD_FLOAT) $(OPENBSD_OPT)"

######################################################################
macosx:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
	CC="$(OSXGCC)" $(MAKEDEFS) \
	MCFLAGS="$(MACOSX_FLOAT) $(MACOSX_OPT)"

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(OSXGCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(MACOSX_FLOAT) $(MACOSX_OPT)"

######################################################################
umacosx:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
	CC="$(UOSXGCC)" $(MAKEDEFS) \
	MCFLAGS="$(MACOSX_FLOAT) $(MACOSX_OPT)"

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(UOSXGCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(MACOSX_FLOAT) $(MACOSX_OPT)"

######################################################################
next:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
	CC="$(GCC)" $(MAKEDEFS) \
	MCFLAGS="$(NEXT_FLOAT) $(NEXT_OPT)"

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(NEXT_FLOAT) $(NEXT_OPT)"


######################################################################
sunpro:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(SUNPRO_CC)" $(MAKEDEFS) \
			MCFLAGS="$(SUNPRO_FLOAT) $(SUNPRO_OPT)" 

	cd hershey/src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(SUNPRO_CC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(SUNPRO_FLOAT) $(SUNPRO_OPT)"

	cd examples; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(SUNPRO_CC)" $(MAKEDEFS)\
			MCFLAGS="$(SUNPRO_FLOAT) $(SUNPRO_OPT)" 


######################################################################
irix5:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(IRIX5CC)" \
			DEVICES="$(DEVICES)" DOBJS="$(DOBJS)" \
			MCFLAGS="$(IRIX5_FLOAT) $(IRIX5_OPT)"

	cd hershey/src; make -f Makefile \
			CC="$(IRIX5CC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(IRIX5_FLOAT) $(IRIX5_OPT)"

######################################################################
sgi-gcc:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" \
			DEVICES="$(DEVICES)" DOBJS="$(DOBJS)" \
			MCFLAGS="$(SGI_GCC_FLOAT) $(SGI_GCC_OPT)"

	cd hershey/src; make -f Makefile \
			CC="$(GCC)" LIBS="$(LIBS)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(SGI_GCC_FLOAT) $(SGI_GCC_OPT)"

######################################################################
aix4:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(AIXCC)" \
			DEVICES="$(DEVICES)" \
			MCFLAGS="$(AIX4_FLOAT) $(AIX4_OPT)" \
			DOBJS="$(DOBJS)"

	cd hershey/src; $(MAKE) -f Makefile \
			CC="$(AIXCC)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(AIX4_FLOAT) $(AIX4_OPT)" \
			LIBS="$(LIBS)" 


######################################################################
aix3-debug:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(AIXCC)" \
			DEVICES="$(DEVICES)" \
			MCFLAGS="-g"\
			DOBJS="$(DOBJS)"

	cd hershey/src; $(MAKE) -f Makefile \
			CC="$(AIXCC)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="-g"\
			LIBS="$(LIBS)" 

######################################################################
aix3:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(AIXCC)" \
			DEVICES="$(DEVICES)" \
			MCFLAGS="$(AIX3_FLOAT) $(AIX3_OPT)" \
			DOBJS="$(DOBJS)"

	cd hershey/src; $(MAKE) -f Makefile \
			CC="$(AIXCC)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(AIX3_FLOAT) $(AIX3_OPT)" \
			LIBS="$(LIBS)" 

######################################################################
sol2sparc:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(SOL2CC)" \
			DEVICES="$(DEVICES)" \
			MCFLAGS="$(SOL2_FLOAT) $(SOL2_OPT)" \
			DOBJS="$(DOBJS)"\

	cd hershey/src; make -f Makefile \
			CC="$(SOL2CC)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(SOL2_FLOAT) $(SOL2_OPT)" \
			LIBS="$(LIBS)" \

######################################################################
sol2x86:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(GCC)" \
			DEVICES="$(DEVICES)" \
			MCFLAGS="$(GCC_FLOAT) $(SOL2X86_OPT)" \
			DOBJS="$(DOBJS)"

	cd hershey/src; make -f Makefile \
			CC="$(GCC)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(GCC_FLOAT) $(SOL2X86_OPT)" \
			LIBS="$(LIBS)" 
######################################################################
osf2:
	cd src; $(MAKE) $(MFLAGS) -f Makefile \
			CC="$(OSF2CC)" \
			DEVICES="$(DEVICES)" \
			MCFLAGS="$(OSF2_FLOAT) $(OSF2_OPT)" \
			DOBJS="$(DOBJS)"

	cd hershey/src; make -f Makefile \
			CC="$(OSF2CC)" \
			FONTLIB="$(FONTLIB)" \
			MCFLAGS="$(OSF2_FLOAT) $(OSF2_OPT)" \
			LIBS="$(LIBS)" 


install:
	cp src/$(LIB) $(DEST)
	chmod 644 $(DEST)/$(LIB)
	$(RANLIB) $(DEST)/$(LIB)

clean:
	cd src; make DOBJS="$(DOBJS)" clean
	cd hershey/src; make FONTLIB="$(FONTLIB)" clean
	cd drivers; make clean
	cd examples; make clean
	rm -f *~ core a.out

clobber:
	cd src; make DOBJS="$(DOBJS)" clobber
	cd hershey/src; make FONTLIB="$(FONTLIB)" clobber
	cd drivers; make clobber
	cd examples; make clobber

quicktar:
	HERE=`basename $$PWD`; \
	touch MANIFEST ; \
        cd .. ; \
	find $$HERE -type f -print |grep -v '.o$$' |grep -v '~$$' |grep -v 'CVS' >$$HERE/MANIFEST ; \
	/usr/gnu/bin/tar  -T $$HERE/MANIFEST -cZvf $$HERE.tar.Z ;


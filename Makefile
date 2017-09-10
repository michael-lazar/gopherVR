
all:
	@echo "Try make {macosxu|macosx11|sun4|irix|irix5|linux|cygwin|sol2sparc|openbsd} for now..."


clean:
	(cd gopher ; $(MAKE) $(MFLAGS) clean ; rm -f config.status config.cache)
	(cd libtracker;	$(MAKE) $(MFLAGS) clean MACHINE=osx)
	(cd libvogl; 	$(MAKE) $(MFLAGS) clean)
	(cd gophervr; 	$(MAKE) $(MFLAGS) clean)
	rm -rf lib/lib* motifmaclauncher/gophervr-mml motifmaclauncher/build motifmaclauncher-gophervr-mml-wtf
	rm -f *~

sun4:
	@echo "*** WARNING: THIS TARGET IS NOT TESTED ***"
	sleep 5
	(cd gopher ; ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=sparc)
	(cd libvogl; 	$(MAKE) $(MFLAGS) sun-gcc)
	(cd gophervr; 	cp Imakefile.sun Imakefile ; xmkmf; $(MAKE) $(MFLAGS))

irix5:
	@echo "*** WARNING: THIS TARGET IS NOT TESTED ***"
	sleep 5
	(cd gopher ; ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=sgi CC=cc)
	(cd libvogl; 	$(MAKE) $(MFLAGS) irix5)
	(cd gophervr; 	cp Imakefile.sgi Imakefile; xmkmf; $(MAKE) $(MFLAGS))


linux:
	(cd gopher ; ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=linux)
	(cd libvogl; 	$(MAKE) $(MFLAGS) linux)
	(cd gophervr; 	cp Imakefile.linux Imakefile ; xmkmf && $(MAKE) $(MFLAGS))

openbsd:
	(cd gopher ; ./configure )
	(cd gopher/object ; $(MAKE) all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=openbsd)
	(cd libvogl;    $(MAKE) $(MFLAGS) openbsd)
	(cd gophervr;   cp Imakefile.openbsd Imakefile ; xmkmf && $(MAKE) $(MFLAGS))


# almost exactly the same
cygwin:
	(cd gopher ; ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=linux)
	(cd libvogl; 	$(MAKE) $(MFLAGS) linux)
	(cd gophervr; 	cp Imakefile.linux Imakefile ; xmkmf && $(MAKE) $(MFLAGS))

sol2sparc:
	@echo "*** WARNING: THIS TARGET IS NOT TESTED ***"
	sleep 5
	(cd gopher ; ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=solaris)
	(cd libvogl; 	$(MAKE) $(MFLAGS) sol2sparc)
	(cd gophervr; 	cp Imakefile.solaris2 Imakefile ; xmkmf; $(MAKE) $(MFLAGS))

macosxu:
	(cd gopher ; CFLAGS="-force_cpusubtype_ALL -mmacosx-version-min=10.4 -arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk" ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=osxu)
	(cd libvogl; 	$(MAKE) $(MFLAGS) umacosx)
	(cd gophervr; 	cp Makefile.uosx Makefile ; $(MAKE) $(MFLAGS))
	(cd motifmaclauncher ; xcodebuild )

macosx11:
	(cd gopher ; CFLAGS="-mmacosx-version-min=10.4 -arch i386" ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=osx)
	(cd libvogl; 	$(MAKE) $(MFLAGS) macosx)
	(cd gophervr; 	cp Makefile.osx Makefile ; $(MAKE) $(MFLAGS))
	# not yet supported for the non-Universal build
	#(cd motifmaclauncher ; xcodebuild )

irix:
	(cd gopher ; ./configure )
	(cd gopher/object ; make all )
	(cd libtracker; $(MAKE) $(MFLAGS) MACHINE=sgidummy CC=cc)
	(cd libvogl; 	$(MAKE) $(MFLAGS) irix5)
	(cd gophervr; 	cp Imakefile.sgi Imakefile; xmkmf && $(MAKE) $(MFLAGS))


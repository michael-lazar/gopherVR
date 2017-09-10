include ../Makefile.config

TARGET = gopher.1 \
	 gophfilt.1 gopherrc.5

install: $(TARGET)
	/usr/bin/install -c -d -m 0755 $(MAN1DIR)
	/usr/bin/install -c -d -m 0755 $(MAN5DIR)
	/usr/bin/install -c -m 0644 gopher.1 $(MAN1DIR)
	/usr/bin/install -c -m 0644 gophfilt.1 $(MAN1DIR)
	/usr/bin/install -c -m 0644 gopherrc.5 $(MAN5DIR)

all:
	$(NROFF)    gopher.1       >gopher.cat
	$(NROFF)    gophfilt.1     >gophfilt.cat
	$(NROFF)    gopherrc.5     >gopherrc.cat

clean:
	-rm -f *~

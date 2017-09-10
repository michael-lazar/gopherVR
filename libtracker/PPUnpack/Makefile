# Edit these options according to your machine
# ppunpack is written in ansi C, with prototypes,
# but it should be straightforward to convert it
# to K&R C

COPTS=-O3
CC=acc
OBJ=depack.o ppunpack.o

all: ppunpack check

ppunpack: $(OBJ)
	$(CC) $(COPTS) -o ppunpack $(OBJ)

sf: ppunpack
	ppunpack sf.pp sf

check: sf
	diff sf sf.chk

.c.o:	
	$(CC) -c $(COPTS) $*.c
clean:
	-rm ppunpack $(OBJ)

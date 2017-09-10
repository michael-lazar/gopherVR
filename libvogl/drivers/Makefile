#
# if your system is running 4.2 or is a sun running
# a sun os before 4.0 you need to add -DBSD to CFLAGS
#

CC = cc

HDRS= ../src/vogl.h

DOBJS = sun.o ps.o hpdxy.o tek.o X11.o

CFLAGS= -I../src $(MCFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $*.c 

all:	$(DOBJS)

$(DOBJS): $(HDRS)

clean:
	rm -f core $(DOBJS) *~

clobber:
	rm -f core *.o
	

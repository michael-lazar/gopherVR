#
# examples makefile 
#
TARGET = Mycube

#SRC = render_dir.c render_kiosk.c render_telnet.c render_document.c render_search.c Mycube.c  
SRC = Mycube.c  
OBJECTS =  Mycube.o
#render_dir.o render_kiosk.o render_telnet.o render_document.o render_search.o

#
# Where to find librarys
#
OURLIBS = ../../lib/libvogl.a ../../lib//libhershey.a
LIBS =  -lXext -lX11 -lm

# On a real SGI...
#OURLIBS = ../hershey/src/libhershey.a
#LIBS = -lgl
#MCFLAGS = -g -float

CFLAGS = -O -g -I../src $(MCFLAGS)



all:	$(TARGET)

$(TARGET): $(OBJECTS) $(OURLIBS) hungarian.h
	$(CC)  $(CFLAGS) $(OBJECTS) -o $@ $(OURLIBS) $(LIBS) 

clean:
	rm -f $(TARGET) $(TARGET).o $(OBJECTS)
	rm -f *~ core

# DO NOT DELETE THIS LINE -- make depend depends on it.

CC = gcc

ifndef O
	O = 3
endif

CFLAGS = -g -O$O -Wall
LDFLAGS += -lodiosacd
VPATH = src
OBJECTS =  main
SUBDIRS = $(VPATH)

.PHONY: all clean install uninstall
all: clean $(OBJECTS) odio-sacd
main: main.c; $(CC) -c $(CFLAGS) $^ -o $(VPATH)/$@.o
odio-sacd: $(foreach object, $(OBJECTS), $(VPATH)/$(object).o); $(CC) -o data/usr/bin/$@ $^ $(LDFLAGS)

clean:

	$(shell rm -f ./data/usr/bin/odio-sacd)
	$(shell rm -f $(foreach librarydir, $(SUBDIRS), $(librarydir)/*.o))

install:

	$(shell install -Dt $(DESTDIR)/usr/bin ./data/usr/bin/odio-sacd)

uninstall:

	$(shell rm -f $(DESTDIR)/usr/bin/odio-sacd)

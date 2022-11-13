CFLAGS ?= -O2
RM ?= rm -f
CPPFLAGS = -Wall -std=c99 -pedantic
INSTALL ?= install
MKDIR   ?= mkdir
STRIP   ?= strip --strip-unneeded -R .comment -R .note -R .note.ABI-tag
DESTDIR ?= /usr/local
BINDIR  ?= $(DESTDIR)/bin

MAJOR_VERSION = 1
MINOR_VERSION = 0
PATCH_VERSION = 0
VERSION = $(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)

TARGET = colorizer
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

#----------------------------------------------------------#

CPPFLAGS += -DAPP_VERSION=$(VERSION)

#----------------------------------------------------------#

all: shared

debug: CFLAGS += -g -D_DEBUG
debug: all

shared: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

static: $(OBJECTS)
	$(CC) -static $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS_STATIC)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

perrorf.o: CPPFLAGS += -D_XOPEN_SOURCE=500 -D_BSD_SOURCE
perrorf.o: perrorf.c perrorf.h
main.o: CPPFLAGS += -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE

distclean: clean

clean:
	$(RM) $(TARGET) $(OBJECTS)

strip: strip-shared

strip-shared: shared
	$(STRIP) --strip-unneeded -R .comment -R .note -R .note.ABI-tag $(TARGET)

strip-static: static
	$(STRIP) --strip-unneeded -R .comment -R .note -R .note.ABI-tag $(TARGET)

install: $(TARGET)
	$(INSTALL) -d $(BINDIR)
	$(STRIP) $(TARGET)
	$(INSTALL) -m 755 $(TARGET) $(BINDIR)

.PHONY: all shared static distclean clean debug

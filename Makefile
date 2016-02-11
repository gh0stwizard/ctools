.PHONY: all clean help install

CFLAGS  ?= -Wall -Wextra -std=c99 -pedantic -static
INSTALL ?= install
MKDIR	?= mkdir
STRIP	?= strip --strip-unneeded -R .comment -R .note -R .note.ABI-tag
DESTDIR ?= /usr/local
BINDIR  ?= $(DESTDIR)/bin

SOURCES	 = $(wildcard *.c)
PROGRAMS = $(patsubst %.c,%,$(SOURCES))

all: $(PROGRAMS)

$(PROGRAMS):
	$(CC) $(CFLAGS) -o $@ $@.c

clean:
	-$(RM) $(PROGRAMS)
	
.SILENT: help
help:
	echo targets: all clean $(PROGRAMS)

install: $(PROGRAMS)
	$(MKDIR) -p $(BINDIR)
	$(STRIP) $(PROGRAMS)
	$(INSTALL) -m 755 $(PROGRAMS) $(BINDIR)

# jff
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo "File(s) with potentially dangerous functions:"
	@egrep $(BADFUNCS) $(SOURCES) || true

.PHONY: all clean help install

LDFLAGS ?= -lm

CFLAGS  ?= -Wall -Wextra -std=c89 -pedantic -static
INSTALL ?= install
MKDIR	?= mkdir
STRIP	?= strip --strip-unneeded -R .comment -R .note -R .note.ABI-tag
DESTDIR ?= /usr/local
BINDIR  ?= $(DESTDIR)/bin

SOURCES	= $(wildcard *.c)
PROGRAM = rpn

all: $(PROGRAM)

$(PROGRAM):
	$(CC) $(CFLAGS) -o $@ $(SOURCES) ${LDFLAGS}

clean:
	-$(RM) $(PROGRAM)
	
.SILENT: help
help:
	echo targets: all check clean install $(PROGRAM)

install: $(PROGRAM)
	$(MKDIR) -p $(BINDIR)
	$(STRIP) $(PROGRAM)
	$(INSTALL) -m 755 $(PROGRAM) $(BINDIR)

# jff
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo "File(s) with potentially dangerous functions:"
	@egrep $(BADFUNCS) $(SOURCES) || true

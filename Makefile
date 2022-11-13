.PHONY: all clean help install

OPTIONS ?= -DUSE_POW
LDFLAGS ?= -lm

CFLAGS  ?= -Wall -Wextra -std=c99 -pedantic -static
INSTALL ?= install
MKDIR	?= mkdir
STRIP	?= strip --strip-unneeded -R .comment -R .note -R .note.ABI-tag
DESTDIR ?= /usr/local
BINDIR  ?= $(DESTDIR)/bin
SOURCES	 = $(wildcard *.c)
PROGRAMS = $(patsubst %.c,%,$(SOURCES))

all: $(PROGRAMS) rpn colorizer

revnum sod htoi loadavg days numconv:
	$(CC) $(CFLAGS) -o $@ $@.c

pow:
	$(CC) $(CFLAGS) ${OPTIONS} -o $@ $@.c $(LDFLAGS)

rpn: rpn/rpn
rpn/rpn:
	$(MAKE) -C rpn

colorizer: colorizer/colorizer
colorizer/colorizer:
	$(MAKE) -C colorizer static

clean:
	-$(RM) $(PROGRAMS)
	$(MAKE) -C rpn clean
	$(MAKE) -C colorizer clean

.SILENT: help
help:
	echo targets: all check clean install $(PROGRAMS) rpn colorizer

install: $(PROGRAMS) install-rpn install-colorizer
	$(MKDIR) -p $(BINDIR)
	$(STRIP) $(PROGRAMS)
	$(INSTALL) -m 755 $(PROGRAMS) $(BINDIR)

install-rpn: rpn
	$(MAKE) -C rpn install

install-colorizer: colorizer
	$(MAKE) -C colorizer install

# jff
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo "File(s) with potentially dangerous functions:"
	@egrep $(BADFUNCS) $(SOURCES) || true

# Makefile for librinterpolate

# we want to use gcc to build and link, but use CC and LD
# if they are defined
CC   ?= $(CC)
CC   ?= gcc
LD_LIBRARY_PATH   ?= /home/izzard/lib:/home/izzard/svn/bin/lib
MAKE    ?= /usr/bin/make
INCDIRS ?=  -I.
LIBS	?=  -lm -lc
LDFLAGS ?=
C_SRC   :=  $(wildcard *.c)
OBJECTS := $(C_SRC:.c=.o)
CFLAGS ?= -O3 $(COPTFLAGS) $(LIBRINTERPOLATE_ARCHFLAGS)
REQCFLAGS := -fPIC -g -std=gnu99 -D__RINTERPOLATE__ -Wall -Wstrict-prototypes -Wno-nonnull-compare -Wformat-signedness -Wformat -D__RINTERPOLATE_BUILD_BUILD_FLAGS__ -Wpedantic
ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

%.o : %.c
	$(CC) -c $(CFLAGS) $(REQCFLAGS) $(INCDIRS) $< -o $@

.PHONY: clean install

librinterpolate: $(OBJECTS)
# make shared library
	$(CC) $(CFLAGS) -shared -Wl,-soname,librinterpolate.so,--export-dynamic -fvisibility=hidden -o librinterpolate.so $(LDFLAGS) $(LIBS) $(OBJECTS)
# make static library
	ar rcs librinterpolate.a $(OBJECTS)
# make test executable
	$(CC) $(CFLAGS) $(REQCFLAGS) -D__TEST_RINTERPOLATE__ test_rinterpolate.c -o test_rinterpolate -lc -lm -ldl $(OBJECTS) -lm
# make config
	$(CC) $(CFLAGS) -D__CONFIG__ -g '-DDESTDIR="$(DESTDIR)"' '-DPREFIX="$(PREFIX)"' rinterpolate-config.c -o rinterpolate-config

# clean objects and shared libraries
clean:
	rm -f $(OBJECTS) librinterpolate.so *.gcda librinterpolate.a test_rinterpolate -r .pgo

# install to PREFIX
# note that we assume ldconfig is in /sbin/
install:
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 librinterpolate.so $(DESTDIR)$(PREFIX)/lib
	install -m 644 librinterpolate.a $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include
	install -m 644 rinterpolate.h $(DESTDIR)$(PREFIX)/include
	install -m 644 rinterpolate_prototypes.h $(DESTDIR)$(PREFIX)/include
	install -m 644 rinterpolate_compiler.h $(DESTDIR)$(PREFIX)/include
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 rinterpolate-config $(DESTDIR)$(PREFIX)/bin

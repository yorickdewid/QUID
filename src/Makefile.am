# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
lib_LTLIBRARIES = libquid.la
libquid_la_LDFLAGS = -module -avoid-version -shared 
libquid_la_CFLAGS = -I$(top_srcdir)/include -Wall -Werror -pedantic -std=c99 \
	-Wmissing-prototypes \
	-Wpointer-arith \
	-Wdeclaration-after-statement \
	-Wendif-labels \
	-Wmissing-format-attribute \
	-Wformat-security \
	-Wno-strict-aliasing
libquid_la_SOURCES=quid.c chacha.c

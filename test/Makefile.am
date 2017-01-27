# Defines which program executables will be produced.
bin_PROGRAMS = chacha quid

# Defines which source files are used to produce the specificed
# executable.
chacha_SOURCES = chacha_test.c
quid_SOURCES = quid_test.c

# Defines flags for the C preprocessor *specific to ``hw``*.
# chacha_CPPLFAGS = 
# quid_CPPLFAGS = 

# These are instead *default* flags, used by all the programs in
# ``bin_PROGRAMS``.
AM_CPPFLAGS = -I$(top_srcdir)/include -DTEST

# Extra options for the linker.
# chacha_LDFLAGS = 
# quid_LDFLAGS = 

# Default options for the linker OR for libtool (depending on what you
# put in ``configure.ac``), used by all the programs in
# ``bin_PROGRAMS``.
#
# In this case, we want to statically link the libraries in
# ``hw_LDADD`.
#AM_LDFLAGS = -shared

# Libraries the ``hw`` binary will be linked to.
chacha_LDADD = $(top_srcdir)/src/libquid.la -L/usr/local/lib
quid_LDADD = $(top_srcdir)/src/libquid.la -L/usr/local/lib

TESTS = chacha quid

# Defines which program executables will be produced.
bin_PROGRAMS = quidsum

# Defines which source files are used to produce the specificed
# executable.
quidsum_SOURCES = quidsum.c

# Defines flags for the C preprocessor *specific to ``hw``*.
# quidsum_CPPLFAGS = 

# These are instead *default* flags, used by all the programs in
# ``bin_PROGRAMS``.
AM_CPPFLAGS = -I$(top_srcdir)/include

# Extra options for the linker.
# quidsum_LDFLAGS = 

# Default options for the linker OR for libtool (depending on what you
# put in ``configure.ac``), used by all the programs in
# ``bin_PROGRAMS``.
#
# In this case, we want to statically link the libraries in
# ``hw_LDADD`.
#AM_LDFLAGS = -shared

# Libraries the ``hw`` binary will be linked to.
quidsum_LDADD = $(top_srcdir)/src/libquid.la -L/usr/local/lib

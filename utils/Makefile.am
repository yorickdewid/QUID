# Defines which program executables will be produced.
bin_PROGRAMS = quid

# Defines which source files are used to produce the specificed
# executable.
quid_SOURCES = quidutil.c

# Defines flags for the C preprocessor *specific to ``hw``*.
# quid_CPPLFAGS = 

# These are instead *default* flags, used by all the programs in
# ``bin_PROGRAMS``.
AM_CPPFLAGS = -I$(top_srcdir)/include

# Extra options for the linker.
# quid_LDFLAGS = 

# Default options for the linker OR for libtool (depending on what you
# put in ``configure.ac``), used by all the programs in
# ``bin_PROGRAMS``.
#
# In this case, we want to statically link the libraries in
# ``hw_LDADD`.
#AM_LDFLAGS = -shared

# Libraries the ``hw`` binary will be linked to.
quid_LDADD = $(top_srcdir)/src/libquid.la -L/usr/local/lib

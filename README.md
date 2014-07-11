QUID
====

QUID or fully Quenza unique identifier is a globally unique number representing an object
or subject. The QUID is based on the OSF UUID version 4, but exceeds in many ways.

The identifiers are generated with additional information such as timestamps, subcategories,
and flags. The Quenza unique identifier is part of the official Quenza project and lays
the foundation for the [Valca](https://github.com/yorickdewid/Valca) database. Both the
library and it's utilities can be used on the GNU Operating system without the need of
additional Quenza software.

Library methods
---------------

The library (libquid) provides an interface for applications to use the Quenza identifier.
Following functions are part of the library:
 * `quid_create()`
 * `quid_get_uid()`
 * `quid_print()`
 * `quid_print_file()`
 * `quid_set_rnd_seed()`
 * `quid_set_mem_seed()`

For additional information see the source code. The library source describes the arguments
each function takes and lists their return type. Also see the example utility on how functions
are used.

The build process
-----------------

QUID is currently using [autotools](https://www.gnu.org/software/autoconf/) for the configuration
and compiling process. In order to configure and make the project the following tools are
required:
 * GNU aclocal
 * GNU autoconf
 * GNU automake
 * GNU libtool
 * GNU make

Most Linux distributions provide these tools under the name `autotools`. Run the commands listed
below to build QUID.
```bash
cd scripts/
./autogen.sh
./configure
make
make install
```

License
-------

QUID, libquid and genquid are released under the terms of the Quenza license.
Copyright (c) 2012-2014

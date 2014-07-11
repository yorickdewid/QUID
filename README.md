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
 * quid_create()
 * quid_get_uid()
 * quid_print()
 * quid_print_file()
 * quid_set_rnd_seed()
 * quid_set_mem_seed()

License
-------

QUID, libquid and genquid are released under the terms of the Quenza license.
Copyright (c) 2012-2014

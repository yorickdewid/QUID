#! /bin/bash

if [[ $(../utils/quid -c 100000 -d 0 | sort | uniq -d) ]]; then
	exit 1
fi

#!/usr/bin/env sh
# Created by Philip R. Simonson

if [ $# -eq 1 ]
then
	make -C $(dirname $0) distclean all && $(dirname $0)/js2bas "$1" > "$1" && exit 0 || exit 1
else
	echo "Usage: $0 <file.js>"
	exit 1
fi

#!/usr/bin/env sh
# Script to automate showing off js2bas.

if [ ! $# -eq 1 ]
then
	echo "Usage: $0 <file.js>"
	exit 1
fi

clear
make -C $(dirname $0) distclean all && $(dirname $0)/js2bas "$1" > test.bas && sudo -E /opt/qb64/qb64

# Cleanup after program.
if [ -x "$(dirname $0)/test" ]
then
	sudo rm $(dirname $0)/test
fi

if [ -x "$(dirname $0)/test2" ]
then
	sudo rm $(dirname $0)/test2
fi

if [ -x "$(dirname $0)/js2bas" ]
then
	make -C $(dirname $0) distclean
fi

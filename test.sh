#!/bin/sh

DIRNAME=`dirname $0`
REALPATH=`which realpath`
if [ ! -z "${REALPATH}" ]; then
	DIRNAME=`realpath ${DIRNAME}`
fi

#Check if environment variable HHVM_EXECUTABLE is set or not
#if not then set it with `which HHVM`
if [ ! -n "$HHVM_EXECUTABLE" ]; then
	HHVM_EXECUTABLE=`which hhvm`
	if [ -z "$HHVM_EXECUTABLE" ]; then
		echo "HHVM is not installed"
		exit 0
	fi
fi

export TEST_PHP_EXECUTABLE=$HHVM_EXECUTABLE

#Check if argument(Directory/testcase) is passed or not.
if [ $# -eq 0 ] ; then
	echo "No arguments are supplied to test(Directory/Testcase)"
	exit 0
fi

if [ ! -f "$DIRNAME/run-tests.php" ] ; then
	echo "run-tests.php doesn't exist"
	exit 0
fi

$TEST_PHP_EXECUTABLE --php -z ${DIRNAME}/aerospike-hhvm.so ${DIRNAME}/run-tests.php $@

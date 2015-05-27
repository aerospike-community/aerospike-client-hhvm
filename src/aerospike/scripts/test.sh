#!/bin/sh
################################################################################
# Copyright 2013-2015 Aerospike, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

CWD=`dirname $0`
AEROSPIKE_EXT_PATH=${CWD}/..
if [ ! -d $AEROSPIKE_EXT_PATH ]; then
    echo "Cannot find the extension source directory at ${AEROSPIKE_EXT_PATH}"
    exit 1
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

if [ ! -f "$AEROSPIKE_EXT_PATH/run-tests.php" ] ; then
	echo "run-tests.php doesn't exist"
	exit 0
fi

$TEST_PHP_EXECUTABLE --php -z ${AEROSPIKE_EXT_PATH}/aerospike-hhvm.so ${AEROSPIKE_EXT_PATH}/run-tests.php $@


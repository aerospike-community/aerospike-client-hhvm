# Aerospike HHVM Client
This is a prototype of [HHVM Native Interface](https://github.com/facebook/hhvm/wiki/Extension-API) for Aerospike Client.


Currently tested on Ubuntu 14.04 LTS 64 bit

## Install HHVM
    wget -O - http://dl.hhvm.com/conf/hhvm.gpg.key | \
    sudo apt-key add -
    echo deb http://dl.hhvm.com/ubuntu trusty main | \
    sudo tee /etc/apt/sources.list.d/hhvm.list
    sudo apt-get update
    sudo apt-get install hhvm-dev

## Install Aerospike C client
Follow the [installation steps](http://www.aerospike.com/download/client/c/3.1.11/) for installing the underlying C client or you may choose to build it from [source](https://github.com/aerospike/aerospike-client-c).

## Build and install Aerospike HHVM client
Edit config.cmake file in the source and modify the following line:
    set(LIBAEROSPIKE, /usr/local)
Replace '/usr/local' with the correct C client installation path (i.e. path to lib/libaerospike.so and include/aerospike)
    hphpize
    cmake . && make;

## Test
Edit tests/aerospike.php and modify the following lines:
   $SERVER_IP = "localhost"
   $SERVER_PORT = 3000;
Replace "localhost" and 3000 with the IP address and port of your Aerospike
server.

    hhvm -d extension_dir=. -d hhvm.extensions[]=aerospike.so tests/aerospike.php

# Aerospike HHVM Client
This is a prototype of an Aerospike client implemented as a
[HHVM Native Interface](https://github.com/facebook/hhvm/wiki/Extension-API)
extension.

Currently tested on 64-bit Ubuntu 14.04 LTS.

## Install HHVM
The HHVM manual provides [installation](http://docs.hhvm.com/manual/en/install-intro.install.php)
and [prebuilt packages](https://github.com/facebook/hhvm/wiki/Prebuilt-Packages-for-HHVM).

## Install Aerospike C client
Get the [latest Aerospike C Client](http://www.aerospike.com/download/client/c/latest/)
library, and install the development package contained in the tar archive.
Alternatively, you can build the C client [from source](https://github.com/aerospike/aerospike-client-c).

For example, on Ubuntu, you would download the tarball, extract it, then:

    sudo dpkg -i aerospike-client-c-devel-3.1.16.ubuntu12.04.x86_64.deb

## Build and install Aerospike HHVM client

    cd src/aerospike
    hphpize
    cmake . && make
    sudo make install

The `make install` step should state the path where the extension is installed.
You will use that path in the next step ([Configure](#configure)).

    vagrant@ubuntu-14:/vagrant/aerospike-client-hhvm/src/aerospike$ sudo make install
    [100%] Built target aerospike-hhvm
    Install the project...
    -- Install configuration: "Debug"
    -- Installing: /usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/aerospike-hhvm.so

If during the build you see an error that complains about the location of Aerospike
header files (for example, `fatal error: aerospike/aerospike_key.h: No such
file or directory`) you will need to edit the `config.cmake` file.

* Edit the target\_link\_libraries to the path containing the C client
  (`libaerospike.so`).
* Edit include\_directories to the directory which contains the C client's
  header files.

## Configure
Add the extension to HHVM by editing /etc/hhvm/php.ini as follows:

    hhvm.extensions[] = /path/to/aerospike-hhvm.so

## Test
Run the unit tests as described in the tests section [README](src/aerospike/tests/README.md).

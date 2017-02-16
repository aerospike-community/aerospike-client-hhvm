# Aerospike HHVM Client
This is a prototype of an Aerospike client implemented as a
[HHVM Native Interface](https://github.com/facebook/hhvm/wiki/Extension-API)
extension.

Currently builds on 64-bit Ubuntu 14.04 LTS against HHVM **3.9.1**, with the
aerospike C client release **3.1.16**. We intend to support HHVM
[LTS releases](https://github.com/facebook/hhvm/wiki/Long-term-support-%28LTS%29#lts-releases).

## Documentation

Documentation of the Aerospike HHVM Client may be found in the [doc directory](doc/README.md).
The API described there is the [specification](doc/aerospike.md) for the PHP Client.
Notes on the internals of the implementation are in [doc/internals.md](doc/internals.md).

[Example PHP code](examples/) can be found in the `examples/` directory.

Full documentation of the Aerospike database is available at http://www.aerospike.com/docs/

## Install HHVM

### [Install HHVM on Ubuntu 14.04 LTS](https://github.com/facebook/hhvm/wiki/Prebuilt-packages-on-Ubuntu-14.04)
```
sudo apt-get install software-properties-common
sudo apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0x5a16e7281be7a449
sudo add-apt-repository "deb http://dl.hhvm.com/ubuntu trusty-lts-3.9 main"
sudo apt-get update
sudo apt-get install -y hhvm
sudo apt-get install -y hhvm-dev
```

### Install HHVM on Other Distros
The HHVM manual provides [prebuilt packages](https://github.com/facebook/hhvm/wiki/Prebuilt-Packages-for-HHVM)
for different distributions. You need to install the `hhvm` and `hhvm-dev`
packages.

HHVM requires `g++` >= 4.8. The individual distribution links in the
[Distro Building Instructions](https://github.com/facebook/hhvm/wiki/Building-and-Installing-HHVM)
article explain how to upgrade to this version in older distros such as
Debian 7.

## Install Aerospike C client
Get the **4.1.3** release of the [Aerospike C Client](http://www.aerospike.com/download/client/c/4.1.3/)
library, and install the development package contained in the tar archive.

For example, on Ubuntu 14.04:

```
wget -O aerospike-c-client.tgz http://www.aerospike.com/download/client/c/4.1.3/artifact/ubuntu14
tar zxvf aerospike-c-client.tgz
cd aerospike-client-c-4.1.3.ubuntu14.04.x86_64
sudo dpkg -i aerospike-client-c-devel-4.1.3.ubuntu14.04.x86_64.deb
```

## Build and Install the Aerospike HHVM Client
In the cloned repo:

```
cd src/aerospike
hphpize
cmake . && make
sudo make install
```

The `make install` step should state the path where the extension is installed.
You will use that path in the next step ([Configure](#configure)). On Ubuntu you
will often see the following:

```
-- Installing: /usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/aerospike-hhvm.so
```

### Troubleshooting

If during the build you see an error that complains about the location of Aerospike
header files (for example, `fatal error: aerospike/aerospike_key.h: No such
file or directory`) you will need to edit the `config.cmake` file.

* Edit the target\_link\_libraries to the path containing the C client
  (`libaerospike.so`).
* Edit include\_directories to the directory which contains the C client's
  header files.

## Configure HHVM to Load the Extension
Add the extension to HHVM by editing /etc/hhvm/php.ini as follows:
```
hhvm.extensions[] = /path/to/aerospike-hhvm.so
```

On Ubuntu this is often:
```
hhvm.extensions[] = /usr/lib/x86_64-linux-gnu/hhvm/extensions/20150212/aerospike-hhvm.so
```

## Test
Run the unit tests as described in the tests section [README](src/aerospike/tests/README.md).

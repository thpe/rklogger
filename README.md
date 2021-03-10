rklogger
========
Copyright 2012-2020 Thomas Petig <foss@petig.eu>
Danfoss, TripleLynx, TLX, UniLynx, ULX and ComLynx are trademarks of Danfoss

![CI](https://github.com/thpe/rklogger/workflows/CI/badge.svg)

Please use the github issue tracker for bug reports and feature requests.

Introduction:
============

rklogger is a command line program for reading the parameters out of Danfoss
inverters. This project is licensed under the GPLv3. It is tested with TLX and
ULX inverters together with Debian stable (x86 and amd64).


Build:
======

Recent versions of:
 * cmake,
 * make,
 * g++ and
 * libboost-program-options-dev
are required.

Please type within the project root directory:
```
git clone https://www.github.com/thpe/rklogger.git
cd rklogger
mkdir build
cd build
cmake ..
make
```

to build the project. Type
```
cmake -DCMAKE_BUILD_TYPE=Debug .
make
```

if you want to compile with additional debug output.
For the installation type:
```
make install
```
or
```
sudo make install
```
depending of the permissions of you current user.

Usage (for more details see the installed manpage):
==================================================

Go into this directory for the following examples, or call the binary from
this path. All numerical parameters are given as hexadecimal number without
prefix, e.g., 4e, 13, or d4.

To read a parameter:
```
rklogger <network> <node> <param_idx> <param_subidx> <param_modulidx>
```

To scan the network:
```
rklogger -s
```

To ping a inverter:
```
rklogger <network> <node>
```

To get information on additional parameters:
```
rklogger --help
```
For details regarding param_idx, param_subidx and param_modulidx please refer
to the ComLynx protocol specification provided by Danfoss.

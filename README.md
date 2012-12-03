Sentio adaptive sampling
========================

Install
-------

### Clone the project

    git clone --recursive git://github.com/mpfusion/sentio-adaptive-sampling.git

Since the project contains the system kernel and drivers as a submodule, the
parameter `--recursive` is necessary.


### Install the tools

For further installation instructions see the [README file of the Sentio
Framework](https://github.com/mpfusion/sentio-framework/blob/master/README.md).


### Run the code

The provided `Makefile` was only tested on a Linux system. For other platforms
the paths of the programs probably need to be adjusted.

A simple `make` will compile all necessary code. A `make flash` will launch
the flash loader (which might need root access, depending on the
configuration) and write the binary blob to the platform.

### Code style

Tabs are used for indentation and spaces for alignment. To maintain a uniform
code appearance the program `astyle` is used with the following options.

	astyle -T4pbcUDH -z2

### TODO

- Divide `Makefile` into two parts for separate compilation of user code and
  system files and move the system `Makefile` into the `sentio-framework` submodule.
- Adjust `Makefile` to support multiple platforms.
- Remove `sudo` call from Makefile.

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


### Documentation

A `make doc` will create the documentation for this project and the Sentio
system. Doxygen needs to be installed. By default a HTML version is generated.
To view the HTML documentation, point the browser to the file
`doc/html/index.html`. For a PDF version LaTeX needs to be installed. To
generate the PDF file, go to the directory `doc/latex` and run `make`.


### Code style

Tabs are used for indentation and spaces for alignment. To maintain a uniform
code appearance the program `astyle` is used with the following options.

	astyle -T4pbcUDH -z2


### TODO

- Adjust `Makefile` to support multiple platforms.
- Remove `sudo` call from Makefile.

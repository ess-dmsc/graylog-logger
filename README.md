# graylog-logger
This is a simple logging library which can be used to send log messages to a Graylog server. This is done by creating messages in the [GELF](http://docs.graylog.org/en/2.1/pages/gelf.html) format and sending them to a Graylog server via TCP. For testing purposes a [Vagrant](https://www.vagrantup.com/) machine running Graylog can be used. A simple Vagrantfile for creating this set-up can be [found here](https://github.com/ess-dmsc/graylog-machine). The argument for creating yet another logging library instead of writing a plugin/sink/handler for an already existing one is that a relatively light weight solution was desired. The library has functionality for writing log messages to console and file as well and by default the library will only write log messages to console.

The repository is split into three parts:

* The logging library.
* Unit tests of the logging library which are completely self contained (i.e. does not require a Graylog server).
* A simple console application which uses the logging library.

A sister project to this library is the Python logging handler [GraylogHandler](https://github.com/ess-dmsc/graylog-handler).

## Requirements
The logging library uses only standard libraries with one exception: [Jsoncpp](https://github.com/open-source-parsers/jsoncpp). This library is included in the project in the form of a header file and an implementation file and will thus not require any packages to be installed.
C++11 features are used extensively and ignoring possible bugs, thread safety is only guaranteed if the compiler used has a correct C++11 (or above) implementation. Although the library should compile on most *nix systems and, small differences in how sockets are handled could introduce bugs. The library also compiles on Windows though it can not yet transmit messages to a Graylog server on this platform.

In order to build the unit tests, the following libraries are also required:

* [boost](http://boost.org)
* [GTest/GMock](https://github.com/google/googletest)

CMake will attempt to download and compile GTest/GMock and thus only boost has to be installed by the developer if the unit tests are to be run.

## Build and install
The library uses [CMake](https://cmake.org) for building. Use the following commands to build everything and to install the library:

```
cd graylog-logger
mkdir build
cd build
cmake ..
make
make install
```

In order to only build the library, set the **BUILD_EVERYTHING** variable to **OFF**, i.e. replace `cmake ..` with:

```
cmake .. -DBUILD_EVERYTHING=OFF
```

### Building on Windows
The compilation of the library has been tested on Windows 10 using the Microsoft Visual C++ compiler (version 14.0). Assuming that CMake as well as Boost are correctly installed and that the appropriate environment variables are configured, the instructions for compiling everything are as follows:

```
cd graylog-logger
mkdir build
cd build
cmake ..
cmake --build .
```

Note that the unit tests are configured to link to the static versions of the Boost libraries.

## Usage
There are currently two alternatives for including this library in your own project.

### 1. Include source files
As the library only depends on standard libraries, the easiest solution is likely to simply copy the implementation files from ```src``` and the directory containing the header files (```graylog_logger```in ```include```) to your own project.

### 2. Use the compiled library
If the instructions under **Build and install** are followed, a compiled binary of the library and the relevant header files will be installed into some system location. Probably ```/usr/local/```. Simply link to the library in the same way as other libraries (e.g. ```-lgraylog_logger```).

### Optional: Using the command line application
If the command line application is compiled, instructions on how to use it are shown if run without any arguments (e.g. ```./console_logger/console_logger```).

### Optional: Running the unit tests
If compiled, the unit tests are run by running the ```unit_tests``` application in the ```unit_tests``` directory of the ```build```directory.

### Code examples
The basic interface to the library is very simple and it should be possible to get a understanding of it by studying the ``Log.hpp`` file. A practical example can be found in the source code for the console application, e.g. ``graylog-logger/console_logger/ConsoleLogger.cpp``. More examples on usage can be found on the ESS wiki page.

## Documentation
The code is currently not documented. Examples illustrating how the library can be used can be found on the ESS wiki and in the `EXAMPLES.md` file.

## To do
The items in the following list is in no particular order. Suggestions and/or patches are welcome.

* Document the code
* Add more advanced logging features
* Clean up the graylog code
* Fix IPv4/IPv6 support
* Add UDP support
* Determine process name
* ...

## Changes

### Version 1.0

* Added support for additional (user defined) fields.
* The library will no longer attempt to connect to a Graylog server on localhost.
* Using code adopted from boost, the library should now be able to determine the current process name.
* The library now builds on Windows and passes all the unit tests. However, during integration testing it failed to correctly send log messages to a Graylog server on this platform. 
* Updated the documentation.
* Minor bug fixes.

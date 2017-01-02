# dm-graylog-logger
This C++ message logging library has been developed for use at the ESS. By default, the library will write log messages to console, file (name: ``messages.log``) and to TCP port 12201 on ``localhost`` in [GELF](http://docs.graylog.org/en/2.1/pages/gelf.html) format. This port will accept log messages from the library if the [dm-graylog-machine](https://bitbucket.org/europeanspallationsource/dm-graylog-machine) [Vagrant](https://www.vagrantup.com/) machine is up and running.

A sister project project to this library is the Python logging handler [GraylogHandler](https://bitbucket.org/europeanspallationsource/grayloghandler).

## Requirements
The logging library uses only standard libraries with one exception: [Jsoncpp](https://github.com/open-source-parsers/jsoncpp). This library is included in the project in the form of a header file and an implementation file and will thus not require any packages to be installed.
C++11 features are used extensively and ignoring possible bugs, thread safety is only guaranteed if the compiler used has a correct C++11 (or above) implementation. Although the library should compile on most *nix systems, small differences in how sockets are handled could introduce bugs. For the same reason, the library will not compile under windows.

In order to build the unit tests, the following libraries are also required:

* [boost](http://boost.org)
* [GTest/GMock](https://github.com/google/googletest)

Note that although the latest release of GTest includes GMock, if your system installs an older version of Gtest you might also have to install GMock separately.

## Build and install
The library uses [CMake](https://cmake.org) for building. Use the following commands to build everything and to install the library:

```
cd dm-graylog-logger
mkdir build
cd build
cmake ..
make
make install
```

In order to only build the library, change the first lint to ```cd dm-graylog-logger/graylog_logger``` and then follow the rest of the instructions. 

## Usage
There are currently two alternatives for including this library in your own project.

### Include source files
As the library only depends on standard libraries, the easiest solution is likely to simply copy the implementation files from ```src``` and the directory containing the header files (```graylog_logger```in ```include```) to your own project.

### Use the compiled library
If the instructions under **Build and install** are followed, a compiled binary of the library and the relevant header files will be installed into some system location. Probably ```/usr/local/```. Simply link to the library in the same way as other libraries (e.g. ```-lgraylog_logger```).

### Using the command line application
If the command line application is compiled, instrucitons on how to use it are shown if run without any arguments (e.g. ```./console_logger/console_logger```).

### Running the unit tests
If compiled, the unit tests are run by running the ```unit_tests``` application in the ```unit_tests``` directory of the ```build```directory.

### Code examples
The basic interface to the library is very simple and it should be possible to get a understanding of it by studying the ``Log.hpp`` file. A practical example can be found in the source code for the console application, e.g. ``dm-graylog-logger/console_logger/ConsoleLogger.cpp``. More examples on usage can be found on the ESS wiki page.

## Documentation
The code is currently not documented. Examples illustrating how the library can be used can be found on the ESS wiki.

## To do
The items in the following list is in no particular order. Suggestions and/or patches are welcome.

* Document the code
* Add more advanced logging features
* Clean up the graylog code
* Fix IPv4/IPv6 support
* Add UDP support
* Determine process name
* ...
# graylog-logger
This is a simple logging library which can be used to send log messages to a Graylog server. This is done by creating messages in the [GELF](http://docs.graylog.org/en/2.1/pages/gelf.html) format and sending them to a Graylog server via TCP. For testing purposes a [Vagrant](https://www.vagrantup.com/) machine running Graylog can be used. A simple Vagrantfile for creating this set-up can be [found here](https://github.com/ess-dmsc/graylog-machine). The argument for creating yet another logging library instead of writing a plugin/sink/handler for an already existing one is that a relatively light weight solution was desired. The library has functionality for writing log messages to console and file as well and by default the library will only write log messages to console.

The repository is split into three parts:

* The logging library.
* Unit tests of the logging library which are completely self contained (i.e. does not require a Graylog server).
* A simple console application which uses the logging library.

A sister project to this library is the Python logging handler [GraylogHandler](https://github.com/ess-dmsc/graylog-handler).

## Requirements
The logging library uses only standard libraries with one exception: [Jsoncpp](https://github.com/open-source-parsers/jsoncpp). This library is included in the project in the form of a header file and an implementation file and will thus not require any packages to be installed.
C++11 features are used extensively and ignoring possible bugs, thread safety is only guaranteed if the compiler used has a correct C++11 (or above) implementation. Although the library should compile on most \*nix systems and, small differences in how sockets are handled could introduce bugs. The library may also compile on Windows (testing welcome 😀).

In order to build the unit tests, the following libraries are also required:

* [boost](http://boost.org)

Conan will automatically download and build Google Test if required. For building it, CMake 2.8.12 or a more recent version is required.

## Build and install
The library uses [Conan](https://conan.io) and [CMake](https://cmake.org) for building. To use Conan, set up the external repositories with the commands

```
conan remote add <remote> https://api.bintray.com/conan/conan-community/conan
conan remote add <remote> https://api.bintray.com/conan/amues/graylog-logger
```
where `<remote>` must be substituted by local names for the repositories.

Use the following commands to build everything and to install the library:

```
mkdir build
cd build
conan install <path_to_src_folder>/conan -o build_everything=True
cmake <path_to_src_folder>
make
make install
```

In order to only build the library, call `conan` without the `build_everything` argument and set the **BUILD_EVERYTHING** variable to **OFF**, i.e. replace `cmake <path_to_src_folder>` with:

```
cmake <path_to_src_folder> -DBUILD_EVERYTHING=OFF
```

## Usage
There are currently two alternatives for including this library in your own project.

### 1. Include it as a Conan Package
In this case, building the library from this repository is not necessary. See an example at https://github.com/ess-dmsc/conan-graylog-logger-test.

### 2. Include source files
As the library only depends on standard libraries, the easiest solution is likely to simply copy the implementation files from ```src``` and the directory containing the header files (```graylog_logger```in ```include```) to your own project.

### 3. Use the compiled library
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

### Version 1.0.2
* Fixed bug related to the order in which static objects are deallocated on some systems. Note to self: premature optimisations are the root of all evil.

### Version 1.0.1
* Fixed serious bug which would cause the graylog interface to send the same message to the server repeatedly without stopping (thanks tijme!).
* Added license file.
* Minor changes to cmake-files (tijme).
* Minor other fixes.

### Version 1.0

* Added support for additional (user defined) fields.
* The library will no longer attempt to connect to a Graylog server on localhost.
* Using code adopted from boost, the library should now be able to determine the current process name.
* The library now builds on Windows and passes all the unit tests. However, during integration testing it failed to correctly send log messages to a Graylog server on this platform.
* Updated the documentation.
* Minor bug fixes.

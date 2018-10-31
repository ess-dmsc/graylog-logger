# graylog-logger

[![DOI](https://zenodo.org/badge/80732581.svg)](https://zenodo.org/badge/latestdoi/80732581)


This is a simple logging library which can be used to send log messages to a Graylog server. This is done by creating messages in the [GELF](http://docs.graylog.org/en/2.1/pages/gelf.html) format and sending them to a Graylog server via TCP. For testing purposes a [Vagrant](https://www.vagrantup.com/) machine running Graylog can be used. A simple Vagrantfile for creating this set-up can be [found here](https://github.com/ess-dmsc/graylog-machine). The argument for creating yet another logging library instead of writing a plugin/sink/handler for an already existing one is that a relatively light weight solution was desired. The library has functionality for writing log messages to console and file as well and by default the library will only write log messages to console.

The repository is split into three parts:

* The logging library.
* Unit tests of the logging library which are completely self contained (i.e. does not require a Graylog server).
* A simple console application which uses the logging library.

A sister project to this library is the Python logging handler [GraylogHandler](https://github.com/ess-dmsc/graylog-handler).

## Requirements
The logging library depends on several external libraries:

* [GTest](https://github.com/google/googletest) (For testing.)
* [JSONForModernCPP](https://github.com/nlohmann/json)
* [ASIO](http://think-async.com) (For networking.)

You will also need CMake (version ≥ 3.9) to build the project. The project makes use of library and language features provided by C++14. It might be possible to link to the library using compilers that only supports C++11 though this has not been tested.

Due to the use of ASIO, the library should compile on most \*nix systems with no issues though only limited testing has been done. The library should also compile and work on Windows though this has not been tested.

## Build and install
The library uses [Conan](https://conan.io) for providing dependencies and [CMake](https://cmake.org) for setting up a build environment. For Conan to be able to download the dependencies, the locations of the dependencies must be specified. Do that by running the following commands:

```
conan remote add community https://api.bintray.com/conan/conan-community/conan
conan remote add ess-dmsc https://api.bintray.com/conan/ess-dmsc/conan
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

To checkout and build the library, run the following commands:

```
git clone https://github.com/ess-dmsc/graylog-logger.git
cd graylog-logger
mkdir build
cd build
conan install .. --build=missing
cmake ..
make install
```

If you do not want to use Conan for providing the dependencies; install them in the way you prefer and simply follow the instructions above but ignore the Conan-step.

## Usage
There are currently two alternatives for including this library in your own project.

### 1. Include it as a Conan Package
In this case, building the library from this repository is not necessary. See an example at https://github.com/ess-dmsc/conan-graylog-logger-test.

### 2. Use the compiled library
If the instructions under **Build and install** are followed, a compiled binary of the library and the relevant header files will be installed into some system location. Probably ```/usr/local/```. Simply link to the library in the same way as other libraries (e.g. ```-lgraylog_logger```).

### Optional: Using the command line application
If the command line application is compiled, instructions on how to use it are shown if run without any arguments (e.g. ```./console_logger/console_logger```).

### Optional: Running the unit tests
If compiled, the unit tests are run by running the ```unit_tests``` application in the ```unit_tests``` directory of the ```build```directory.

### Code examples
The basic interface to the library is very simple and it should be possible to get a understanding of it by studying the ``Log.hpp`` file. A practical example can be found in the source code for the console application, e.g. ``graylog-logger/console_logger/ConsoleLogger.cpp``. More examples on usage can be found on the ESS wiki page.

## Documentation
The code has some documentation. To generate it, run _doxygen_ in the root of the repository i.e.:

```
cd graylog-logger
doxygen
```

Examples illustrating how the library can be used can be found in the `EXAMPLES.md` file.

## To do
The items in the following list is in no particular order. Suggestions and/or patches are welcome.

* Improved threading model
* Performance testing and improvements
* Version number from git tag
* Log file rotation
* UDP Messages
* Integrate the fmt library?
* Add example logging macros

## Changes

### Version 1.1.0
* Completely removed the dependency on boost for unit testing.
* Switched out the networking code for code using ASIO.
* Updated to CMake code to use modern CMake features.
* Moved towards using the LLVM coding standard. **This has changed the interface, you might have to update your code.** 
* Switched to using the JSONForModernCPP library. This library is not included in the repository.
* Added the `Info` severity level which is exactly the same as the `Informational` severity level.
* Updated the CI-code (Jenkins) to build the library on more (modern) operating systems.
* Messages are now passed to the graylog-server with millisecond resolution timestamps.
* The code now requires C++14 to compile.
* Updated the documentation.
* Modernized the code somewhat.

### Version 1.0.5
* Improved the code based on clang-tidy static analysis results.
* (Very) minor changes to the interface.

### Version 1.0.4
* Switched to using position independent code (-fPIC). When building the static version of the library.
* Updated the Jenkins-script.

### Version 1.0.3
* Fixed serious excessive CPU usage bug and related memory leak in the Graylog server conenction code.
* Added support for Conan (Afonso).
* Re-did formating of the code (Afonso).
* Added automatic checking of code in Jenkinsfile (Afonso).

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

[![DOI](https://zenodo.org/badge/80732581.svg)](https://zenodo.org/badge/latestdoi/80732581)

# Graylog logger

This is a simple logging library which can be used to send log messages to a Graylog server. This is done by creating messages in the [GELF](http://docs.graylog.org/en/2.1/pages/gelf.html) format and sending them to a Graylog server via TCP. For testing purposes a [Vagrant](https://www.vagrantup.com/) machine running Graylog can be used. A simple Vagrantfile for creating this set-up can be [found here](https://github.com/ess-dmsc/graylog-machine). The argument for creating yet another logging library instead of writing a plugin/sink/handler for an already existing one is that a relatively light weight solution was desired. The library has functionality for writing log messages to console and file as well and by default the library will only write log messages to console.

The repository is split into three parts:

* The logging library.
* Unit tests of the logging library which are completely self contained (i.e. does not require a Graylog server).
* A simple console application which uses the logging library.

A sister project to this library is the Python logging handler [GraylogHandler](https://github.com/ess-dmsc/graylog-handler).

- [Further documentation](documentation/README.md)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites


The logging library depends on several external libraries:

* [GTest](https://github.com/google/googletest) (For testing.)
* [JSONForModernCPP](https://github.com/nlohmann/json)
* [ASIO](http://think-async.com) (For networking.)

You will also need CMake (version ≥ 3.9) to build the project. The project makes use of library and language features provided by C++14. It might be possible to link to the library using compilers that only supports C++11 though this has not been tested.

Due to the use of ASIO, the library should compile on most \*nix systems with no issues though only limited testing has been done. The library should also compile and work on Windows though this has not been tested.


### Installing

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


#### Documentation
The code has some documentation. To generate it, run _doxygen_ in the root of the repository i.e.:

```
cd graylog-logger
doxygen
```

## Running the tests
If compiled, the unit tests are run by running the ```unit_tests``` application in the ```unit_tests``` directory of the ```build```directory.

## Deployment

Examples illustrating how the library can be used can be found in the [examples.md](documentation/examples.md) file.

## Built With
* [CMAKE](https://cmake.org/) - Cross platform makefile generation
* [Conan](https://conan.io/) - Package manager for C++

## Contributing
TBD

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [releases on github](https://github.com/ess-dmsc/graylog-logger/releases).

[See description of changes](documentation/changes.md)

## Authors

* **Jonas Nilsson** - *Initial work* - [SkyToGround](https://github.com/SkyToGround)

See also the list of [contributors](https://github.com/ess-dmsc/graylog-logger/graphs/contributors) who participated in this project.

## License

This project is licensed under the BSD-2 License - see the [LICENSE.md](LICENSE.md) file for details

# Graylog logger

This is a simple logging library which can be used to send log messages to a Graylog server. 
This is done by creating messages in the [GELF](http://docs.graylog.org/en/2.1/pages/gelf.html) format and sending them to a Graylog server via TCP. 
The argument for creating yet another logging library instead of writing a plugin/sink/handler for an already existing one is that a relatively light weight solution was desired. 
The library has functionality for writing log messages to console and file as well. 
By default the library will only write log messages to console.

The repository is split into four parts:

* The logging library.
* Unit tests of the logging library which are completely self contained (i.e. does not require a Graylog server).
* A simple console application which uses the logging library.
* Some benchmarking code which is used for profiling and optimising the code as well as test for performance regression.

- Further documentation can be [found here.](documentation/README.md)

## Installation

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
See deployment for notes on how to deploy the project on a live system.


## Requirements

- **C++14 compiler** (we provide conan profiles for gcc11)
- **CMake 3.15+**
- **Conan 1.66+**

## Docker image


## Dependencies

All dependencies are listed in [`conanfile.py`](./conanfile.py) and automatically installed via Conan.

#### Install Conan configuration
```
conan config install http://github.com/ess-dmsc/conan-configuration.git
```

#### Build with Conan
```
git clone https://github.com/ess-dmsc/graylog-logger.git
cd graylog-logger
conan install . --build=missing -pr=linux_x86_64_gcc11
cmake -B build/Release -S . -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release
```

#### Making a Conan package

```
conan create . ess-dmsc/testing -pr=linux_x86_64_gcc11
```

#### Documentation
The code has some documentation. To generate it, run _doxygen_ in the root of the repository i.e.:

```
cd graylog-logger
doxygen
```

## Running the tests

```
cmake --build build/Release --target unit_tests --target performance_test

# unit tests
ctest --test-dir build/Release/

# performance tests
./build/Release/performance_test/performance_test
```

## Deployment

Examples illustrating how the library can be used can be found in the [examples.md](documentation/examples.md) file.

## Contributing

Get in contact by [creating an issue](https://github.com/ess-dmsc/graylog-logger/issues). I will be happy to assist you making the changes that you want.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [releases on github](https://github.com/ess-dmsc/graylog-logger/releases).

[See description of changes](documentation/changes.md)

## Authors

* **Jonas Nilsson** - *Initial work* - [SkyToGround](https://github.com/SkyToGround)
* **Afonso Mukai** - Provided much assistance in setting up the continous integration system.
* **Matthew Jones** - Improved the CMake code and suggested improvements to the interface. 

See also the list of [contributors](https://github.com/ess-dmsc/graylog-logger/graphs/contributors) to this project.

## License

This project is licensed under the BSD-2 License - see the [LICENSE.md](LICENSE.md) file for details

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

Due to the use of ASIO, the library should compile on most \*nix systems and Windows 10 with no issues though only limited testing has been done.


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
conan install .. --build=outdated
cmake ..
make install
```


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

### Creating a new conan package
If you have made changes to the *graylog-logger* library and subsequently also want to update the conan-package, follow these instructions:

1. Edit line 7 of the *conanfile.py*-file to set the version of the new conan package. This version should be the same as the the one indicated by the *graylog-logger* tag.

2. Edit line 15 of the *conanfile.py*-file in this repository to checkout the tag or commit of *graylog-logger* that you want to package.

3. When in the directory of the local copy of *conan-graylog-logger*, execute this command:

	```
	conan create . graylog-logger/x.y.z-dm1@ess-dmsc/stable
	```
	Where **x.y.z-dm1** is the same version string as set on line 7 in the *conanfile.py*-file.

4. Upload the new package to the relevant conan package repository by executing:

	```
	conan upload graylog-logger/x.y.z-dm1@ess-dmsc/stable --remote alias_of_repository
	```

	Where **x.y.z-dm1** is the version of the conan package as mentioned above and **alias\_of\_repository** is exactly what it says. You can list all the repositories that your local conan installation is aware of by running: `conan remote list`.

## Authors

* **Jonas Nilsson** - *Initial work* - [SkyToGround](https://github.com/SkyToGround)

See also the list of [contributors](https://github.com/ess-dmsc/graylog-logger/graphs/contributors) who participated in this project.

## License

This project is licensed under the BSD-2 License - see the [LICENSE.md](LICENSE.md) file for details

## Usage
There are currently two alternatives for including this library in your own project.

### 1. Include it as a Conan Package
In this case, building the library from this repository is not necessary. See an example at https://github.com/ess-dmsc/conan-graylog-logger-test.

### 2. Use the compiled library
If the instructions under **Build and install** are followed, a compiled binary of the library and the relevant header files will be installed into some system location. Probably ```/usr/local/```. Simply link to the library in the same way as other libraries (e.g. ```-lgraylog_logger```).

### Optional: Using the command line application
If the command line application is compiled, instructions on how to use it are shown if run without any arguments (e.g. ```./console_logger/console_logger```).



### Code examples
The basic interface to the library is very simple and it should be possible to get a understanding of it by studying the ``Log.hpp`` file. A practical example can be found in the source code for the console application, e.g. ``graylog-logger/console_logger/ConsoleLogger.cpp``. More examples on usage can be found on the ESS wiki page.

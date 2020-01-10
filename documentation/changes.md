## Changes

### Version 2.0.0
* Added performance tests.
* Replaced the home-brewed concurrent queue with a *much* faster open source one.
* Made the logger run in a separate thread.
* Added flushing functionality, see documentation for details in how it works.
* Added support for the fmtlib (for formatting) if present. *Note:* The formatting is done in a separate thread which should in most cases be faster than doing it in the thread that calls the logging library.
* Improved/fixed the README-file.
* Made it easier/possible to build the library without using Conan for providing dependencies.
* Added and fixed examples.
* Fixed the non-working `emptyQueue()` and `queueSize()` implementations.
* Fixed/improved code documentation.
* De-activated the use of clang-format in our continuous integration system. It will be re-activated when we have updated clang-format on our build nodes to a newer version.
* Removed the message queue size parameter from log message handlers as it was deemed unnecessary.
* Fixed and added unit tests.

### Version 1.2.0
* Made the library compile with MSVC on Windows.

### Version 1.1.8
* Fixed a bug that prevented the library from being included as a conan package in other projects.
* Moved some of the code from externally facing header files to internal header files. This reduces the number of includes.
* Other minor CMake and code fixes.

### Version 1.1.7
* Updated the JSON for Modern C++ from 3.1.0 to 3.6.1.

### Version 1.1.6
* ASIO has been updated from 1.12.0 to 1.13.0.

### Version 1.1.5
* Improved CMake/Conan interaction.

### Version 11.4
* Build system fixes.

### Version 1.1.3
* Fixed serious issue that would cause the library to repeatedly cause new connections to spawn.
* Made minor changes to the documentation.
* Minor other changes.

### Version 1.1.2
* Some indication of the result of opening a log file was required and that has now been added.
* The threading model for the ConsoleLogger and FileLogger handlers has been updated and is now much improved.

### Version 1.1.1
* Made the static version of the library compile with position independent code (`-fPIC`).

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
* **This is the last version not using semantic versioning. The next release which has breaking changes in the API will have the version number 2.0.0.**

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

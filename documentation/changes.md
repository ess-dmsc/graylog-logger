## Changes

### Version 1.2.0
* Made the library compile with MSVC on Windows.

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

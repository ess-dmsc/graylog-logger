from conans import ConanFile, CMake, tools
from conans.errors import ConanException
import os


class GraylogloggerConan(ConanFile):
    name = "graylog-logger"
    version = "1.0.2"
    license = "BSD 2-Clause"
    url = "https://bintray.com/amues/graylog-logger"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "build_everything": [True, False]
    }
    default_options = "shared=True\n" \
                      "build_everything=True"
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/ess-dmsc/graylog-logger.git")
        self.run("cd graylog-logger && git checkout conan")
        tools.replace_in_file("graylog-logger/CMakeLists.txt", "PROJECT(dm-graylog-logger)", '''PROJECT(dm-graylog-logger)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()''')

    def build(self):
        cmake = CMake(self)

        shared = "-DBUILD_SHARED_LIBS=ON" if self.options.shared else ""
        build_everything = "-DBUILD_EVERYTHING=OFF" if not self.options.build_everything else ""
        definitions = ' '.join([shared, build_everything])

        try:
            self.output.info("Try to run cmake3")
            self.run("cmake3 --version")
            cmake_command = "cmake3"
        except ConanException:
            self.output.info("Using cmake instead of cmake3")
            cmake_command = "cmake"

        self.run('%s graylog-logger %s %s' % (cmake_command, cmake.command_line, definitions))
        self.run("%s --build . %s" % (cmake_command, cmake.build_config))

        if self.options.build_everything:
            self.run("./unit_tests/unit_tests")

    def package(self):
        self.copy("*.h", dst="include/graylog_logger", src="graylog-logger/include/graylog_logger")
        self.copy("*.hpp", dst="include/graylog_logger", src="graylog-logger/include/graylog_logger")
        if self.settings.os == "Macos":
            self.copy("*.dylib", dst="lib", keep_path=False)
        else:
            self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["graylog_logger"]

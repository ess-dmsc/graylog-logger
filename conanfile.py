import os
from conans import ConanFile, CMake, tools


class GraylogloggerConan(ConanFile):
    name = "graylog-logger"
    version = "1.1.7-dm1"
    license = "BSD 2-Clause"
    url = "https://bintray.com/ess-dmsc/graylog-logger"
    requires = ("jsonformoderncpp/3.6.1@vthiery/stable", "asio/1.13.0@bincrafters/stable", "gtest/1.8.1@bincrafters/stable", "cmake_installer/3.10.0@conan/stable")
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "virtualrunenv"
    default_options = {"gtest:shared":True, }

    def source(self):
        self.run("git clone https://github.com/ess-dmsc/graylog-logger.git")
        self.run("cd graylog-logger && git checkout v1.1.7")
    
    def _configure_cmake(self):
        cmake = CMake(self, parallel=True)

        cmake.definitions["BUILD_EVERYTHING"] = "OFF"
        if tools.os_info.is_macos:
            cmake.definitions["CMAKE_MACOSX_RPATH"] = "ON"
            cmake.definitions["CMAKE_SHARED_LINKER_FLAGS"] = "-headerpad_max_install_names"
        cmake.configure(source_dir=self.name, build_dir=".")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        
        cmake.build()

        if tools.os_info.is_macos:
            os.system("install_name_tool -id '@rpath/libgraylog_logger.dylib' "
                      "graylog_logger/libgraylog_logger.dylib")

        os.rename(
            "graylog-logger/LICENSE.md",
            "graylog-logger/LICENSE.graylog-logger"
        )

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
        
        self.copy("LICENSE.*", src="graylog-logger")

    def package_info(self):
        self.cpp_info.libs = ["graylog_logger"]

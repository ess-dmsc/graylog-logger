from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain

class GraylogLoggerConan(ConanFile):
    name = "graylog-logger"
    version = "2.1.5"
    license = "BSD 2-Clause"
    url = "https://gitlab.esss.lu.se/ecdc/ess-dmsc/graylog-logger"
    description = "A simple logging library with support for pushing messages to a graylog-logger service."
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "console_logger/*", "unit_tests/*", "performance_test/*", "conanfile.py"
    default_options = {
        "gtest:shared": False,
    }

    requires = (
        "gtest/1.11.0",
        "asio/1.22.1",
        "nlohmann_json/3.10.5",
        "benchmark/1.6.1",
        "concurrentqueue/1.0.3",
        "fmt/8.1.1",
    )
    
    generators = ("cmake_find_package")

    def layout(self):
        self.folders.source = "."
        self.folders.build = "."

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_EVERYTHING"] = "ON"
        tc.variables["CONAN_RUN"] = "ON" 
        tc.generate()

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake
        
    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        self.copy("LICENSE.*", dst="licenses", src=self.source_folder)

    def package_info(self):
        self.cpp_info.libs = ["graylog_logger"]

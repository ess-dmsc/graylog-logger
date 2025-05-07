from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout

class GraylogLoggerConan(ConanFile):
    name = "graylog-logger"
    version = "2.1.6"
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
    
    generators = "CMakeDeps", "CMakeToolchain"

    def layout(self):
        cmake_layout(self)
        
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        self.copy("LICENSE.*", dst="licenses", src=self.source_folder)

    def package_info(self):
        self.cpp_info.set_property("cmake_target_name", "GraylogLogger::graylog_logger")
        self.cpp_info.libs = ["graylog_logger"]
        self.cpp_info.set_property("cmake_file_name", "graylog-logger")

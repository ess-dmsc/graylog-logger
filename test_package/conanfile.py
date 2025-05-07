import os
from conans import ConanFile
from conan.tools.cmake import CMake
from conan.tools.build import cross_building

class GraylogLoggerTestPackage(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    test_type = "explicit"
    requires = "graylog-logger/2.1.6@ess-dmsc/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if not cross_building(self):
            for root, _, files in os.walk(self.build_folder):
                if "test_package" in files:
                    exe_path = os.path.join(root, "test_package")
                    self.output.info(f"Running test binary: {exe_path}")
                    self.run(exe_path, env="conanrun")
                    return
            raise Exception("test_package binary not found!")

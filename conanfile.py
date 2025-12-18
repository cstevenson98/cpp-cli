from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps


class CppCliConan(ConanFile):
    name = "cpp-cli"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*"

    def requirements(self):
        self.requires("gtest/1.15.0")

    def layout(self):
        build_type = str(self.settings.build_type)
        self.folders.build = f"build/{build_type}"
        self.folders.generators = f"build/{build_type}/generators"

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_target_name", "cpp-cli::cpp-cli")
        self.cpp_info.components["core_cli"].libs = ["core_cli"]
        self.cpp_info.components["core_cli"].set_property("cmake_target_name", "cpp-cli::core_cli")

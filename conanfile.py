from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps


class CppCliConan(ConanFile):
    name = "cpp-cli"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("gtest/1.15.0")

    def layout(self):
        # Simple layout: generators go directly in output folder
        build_type = str(self.settings.build_type)
        self.folders.generators = f"build/{build_type}/generators"
        self.folders.build = f"build/{build_type}"

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = "ON"
        tc.generate()

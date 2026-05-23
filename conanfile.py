from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd
from conan.tools.scm import Version
from conan.errors import ConanInvalidConfiguration

required_conan_version='>=2.28'

class LisaConan(ConanFile):
    name = "lisa"
    version = "2.0.0"
    license = "MIT"
    author = "kinami"
    settings = "os", "compiler", "build_type", "arch"

    default_options = {
        "reflectcpp/*:with_xml": True,
        "boost/*:shared": False,
        "spdlog/*:shared": False,
        "sdl/*:shared": False,
        "miniz/*:shared": False,
        "pugixml/*:shared": False,
        "ktx/*:shared": False,
    }

    def validate(self):
        cppstd = self.settings.compiler.get_safe("cppstd")
        if cppstd and int(str(cppstd).replace("gnu", "")) < 20:
            raise ConanInvalidConfiguration("C++20 or higher required")

    def requirements(self):
        self.requires("boost/1.86.0")
        self.requires("cli11/2.4.2")
        self.requires("entt/3.15.0")
        self.requires("fastgltf/0.8.0")
        self.requires("glm/1.0.1")
        self.requires("ktx/4.4.2")
        self.requires("pugixml/1.14")
        self.requires("reflect-cpp/0.23.0")
        self.requires("spdlog/1.15.0")
        self.requires("stb/cci.20230920")
        self.requires("tinyexr/1.0.7")
        self.requires("tinyobjloader/2.0.0-rc10")
        self.requires("vulkan-memory-allocator/3.3.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

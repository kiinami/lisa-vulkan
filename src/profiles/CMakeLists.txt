# Dependencies ################################################################

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Files and paths #############################################################

set(VULKAN_REGISTRY_PATH "${VULKAN_SDK}/share/vulkan/registry")

find_file(VK_GEN_PROFILES_SCRIPT
        NAMES gen_profiles_solution.py
        PATHS "${VULKAN_REGISTRY_PATH}"
        REQUIRED
        NO_DEFAULT_PATH
)

find_file(VK_XML_FILE
        NAMES vk.xml
        PATHS "${VULKAN_REGISTRY_PATH}"
        REQUIRED
        NO_DEFAULT_PATH
)

set(PROFILES_JSON "${CMAKE_CURRENT_SOURCE_DIR}/profiles.json")
set(PROFILE_OUT_INC "${CMAKE_CURRENT_BINARY_DIR}/include")
set(PROFILE_OUT_SRC "${CMAKE_CURRENT_BINARY_DIR}/src")

set(GENERATED_HEADERS
        "${PROFILE_OUT_INC}/vulkan/vulkan_profiles.h"
        "${PROFILE_OUT_INC}/vulkan/vulkan_profiles.hpp"
)
set(GENERATED_SOURCES
        "${PROFILE_OUT_SRC}/vulkan_profiles.cpp"
)

# Custom command ##############################################################

add_custom_command(
        OUTPUT ${GENERATED_HEADERS} ${GENERATED_SOURCES}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROFILE_OUT_INC}/vulkan" "${PROFILE_OUT_SRC}"
        COMMAND Python3::Interpreter "${VK_GEN_PROFILES_SCRIPT}"
        --registry "${VK_XML_FILE}"
        --input "${CMAKE_CURRENT_SOURCE_DIR}"
        --input-filenames "profiles.json"
        --api vulkan
        --output-library-inc "${PROFILE_OUT_INC}/vulkan"
        --output-library-src "${PROFILE_OUT_SRC}"
        DEPENDS "${PROFILES_JSON}"
        "${VK_GEN_PROFILES_SCRIPT}"
        "${VK_XML_FILE}"
        COMMENT "Generating Vulkan Profiles library from profiles.json using SDK tool"
        VERBATIM
)

# Library target ##############################################################

add_library(vulkan_profiles STATIC ${GENERATED_SOURCES} ${GENERATED_HEADERS})

target_include_directories(vulkan_profiles PUBLIC "${PROFILE_OUT_INC}")

target_link_libraries(vulkan_profiles PUBLIC Vulkan::Vulkan)

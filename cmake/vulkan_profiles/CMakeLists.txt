# cmake/vulkan_profiles/CMakeLists.txt

###############################################################################
# Fetch the source
###############################################################################
include(FetchContent)
FetchContent_Declare(
        vulkan_profiles_repo
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Profiles.git
        GIT_TAG        main
)
FetchContent_GetProperties(vulkan_profiles_repo)
if(NOT vulkan_profiles_repo_POPULATED)
    message(STATUS "Fetching Vulkan Profiles source...")
    FetchContent_Populate(vulkan_profiles_repo)
endif()

###############################################################################
# Find Python and Vulkan Registry
###############################################################################
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(Vulkan REQUIRED)

get_target_property(VULKAN_INCLUDE_DIRS Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
set(VULKAN_REGISTRY_PATH "")
foreach(INC_DIR ${VULKAN_INCLUDE_DIRS})
    if(EXISTS "${INC_DIR}/../share/vulkan/registry/vk.xml")
        set(VULKAN_REGISTRY_PATH "${INC_DIR}/../share/vulkan/registry/vk.xml")
        break()
    elseif(EXISTS "${INC_DIR}/vulkan/registry/vk.xml")
        set(VULKAN_REGISTRY_PATH "${INC_DIR}/vulkan/registry/vk.xml")
        break()
    endif()
endforeach()

###############################################################################
# Generation
###############################################################################
set(CLEAN_PROFILES_DIR "${CMAKE_CURRENT_BINARY_DIR}/clean_profiles")
file(REMOVE_RECURSE "${CLEAN_PROFILES_DIR}")
file(MAKE_DIRECTORY "${CLEAN_PROFILES_DIR}")

file(GLOB RAW_PROFILE_FILES "${vulkan_profiles_repo_SOURCE_DIR}/profiles/VP_*.json")
foreach(PROFILE_FILE ${RAW_PROFILE_FILES})
    if(NOT "${PROFILE_FILE}" MATCHES ".*_config\\.json$")
        file(COPY "${PROFILE_FILE}" DESTINATION "${CLEAN_PROFILES_DIR}")
    endif()
endforeach()

set(VP_GEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${VP_GEN_OUTPUT_DIR}/vulkan")

message(STATUS "Generating Vulkan Profiles header...")
execute_process(
        COMMAND ${Python3_EXECUTABLE} "${vulkan_profiles_repo_SOURCE_DIR}/scripts/gen_profiles_solution.py"
        --registry "${VULKAN_REGISTRY_PATH}"
        --input "${CLEAN_PROFILES_DIR}"
        --output-library-inc "${VP_GEN_OUTPUT_DIR}/vulkan"
        RESULT_VARIABLE gen_res
        OUTPUT_VARIABLE gen_out
        ERROR_VARIABLE gen_err
)

if(NOT gen_res EQUAL 0)
    message(STATUS "Generator Output: ${gen_out}")
    message(STATUS "Generator Error: ${gen_err}")
    message(FATAL_ERROR "Vulkan Profiles generation failed!")
endif()

##########################################################################
# Target
###############################################################################
add_library(VulkanProfiles_Generated INTERFACE)
target_include_directories(VulkanProfiles_Generated INTERFACE "${VP_GEN_OUTPUT_DIR}")
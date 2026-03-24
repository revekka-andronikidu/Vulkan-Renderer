set(TARGET_NAME "GP2-Andronikidu-Revekka")

#download external libraries
include(FetchContent)

FetchContent_Declare(
   GLFW
   GIT_REPOSITORY https://github.com/glfw/glfw.git
   GIT_TAG 3.4
   GIT_SHALLOW TRUE
   GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(GLFW)

FetchContent_Declare(
    GLM
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(GLM)

#make sure vulkan is installed
find_package(Vulkan REQUIRED)

add_executable(${TARGET_NAME} main.cpp)

#link libraries
target_link_libraries(${TARGET_NAME} PRIVATE Vulkan::Vulkan glfw glm::glm)

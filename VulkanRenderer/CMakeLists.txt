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

add_executable(${TARGET_NAME} 
main.cpp
VulkanRenderer.cpp
Vulkan/SwapChain.cpp
Vulkan/CommandPool.cpp
Vulkan/Pipeline.cpp
Vulkan/Resources/Buffer.cpp
Vulkan/Resources/Image.cpp
Vulkan/Resources/ResourceManager.cpp
Vulkan/Resources/Mesh.cpp
Vulkan/Resources/Material.cpp
Vulkan/Resources/DescriptorPool.cpp "Vulkan/UniformBuffer.cpp" "Vulkan/VulkanContext/Device.cpp")

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${TARGET_NAME} PRIVATE Vulkan::Vulkan glfw glm::glm)

###SHADER COMPILATION###
#find shaders
file(GLOB_RECURSE SHADERS
    ${CMAKE_SOURCE_DIR}/resources/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/resources/shaders/*.frag
)
message("Found shaders: ${SHADERS}")

set(SPIRV_FILES)
foreach(SHADER ${SHADERS})

    get_filename_component(FILE_NAME ${SHADER} NAME)

    #get relative path to shader file
    file(RELATIVE_PATH relativePath ${CMAKE_SOURCE_DIR}/resources/shaders/ ${SHADER})

     # output in build dir
    set(spirvOutput ${CMAKE_BINARY_DIR}/resources/shaders/${relativePath}.spv)   

    #make sure output directory exists
    get_filename_component(outputDir ${spirvOutput} DIRECTORY)
    file(MAKE_DIRECTORY ${outputDir})

    add_custom_command(
        OUTPUT ${spirvOutput}
        COMMAND glslangValidator -V -g ${SHADER} -I${CMAKE_SOURCE_DIR}/resources/shaders -o ${spirvOutput}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILE_NAME} to SPIR-V"
    )

    list(APPEND SPIRV_FILES ${spirvOutput})
endforeach()

#shader build target
add_custom_target(CompiledShaders ALL DEPENDS ${SPIRV_FILES})

#make add_executable depend on shaders
add_dependencies(${TARGET_NAME} CompiledShaders)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${TARGET_NAME}>/resources
    COMMENT "Copying resources to output directory"
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_BINARY_DIR}/resources/shaders
    $<TARGET_FILE_DIR:${TARGET_NAME}>/resources/shaders
    COMMENT "Copying compiled shaders"
)







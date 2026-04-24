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
Vulkan/VulkanRenderer.cpp
Vulkan/Core/Device.cpp
Vulkan/Rendering/SwapChain.cpp
Vulkan/Rendering/Pipeline.cpp
Vulkan/Frame/UniformBuffer.cpp
Vulkan/Frame/CommandPool.cpp
Vulkan/Resources/Buffer.cpp
Vulkan/Resources/Image.cpp
Vulkan/Resources/ResourceManager.cpp
Vulkan/Resources/Scene/Mesh.cpp
"Vulkan/Resources/Scene/TextureArray.cpp"
Vulkan/Rendering/DescriptorPool.cpp
 Vulkan/Resources/Scene/MeshLoader.cpp)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${TARGET_NAME} PRIVATE Vulkan::Vulkan glfw glm::glm)

# ----------------------------------------------
# Sponza Scene — download once, skip if present
# ----------------------------------------------
set(SPONZA_DEST "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources/models/Sponza")
set(SPONZA_MARKER "${CMAKE_BINARY_DIR}/sponza_downloaded.stamp")

if(NOT EXISTS "${SPONZA_MARKER}")
    message(STATUS "Sponza not found — downloading from KhronosGroup...")

    FetchContent_Declare(
        SponzaScene
        URL "https://github.com/KhronosGroup/glTF-Sample-Assets/archive/refs/heads/main.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_GetProperties(SponzaScene)
    if(NOT sponzascene_POPULATED)
        FetchContent_Populate(SponzaScene)
    endif()

    # Copy only the Sponza glTF folder — skip the rest of the huge repo
    file(COPY "${sponzascene_SOURCE_DIR}/Models/Sponza/glTF/"
         DESTINATION "${CMAKE_BINARY_DIR}/resources/models/Sponza")

    # Write stamp so we don't re-download on every cmake configure
    file(WRITE "${SPONZA_MARKER}" "downloaded")
    message(STATUS "Sponza assets copied to ${CMAKE_BINARY_DIR}/resources/models/Sponza")
else()
    message(STATUS "Sponza already downloaded — skipping")
endif()

# ----------------------------------------------
# Shader Compilation
# ----------------------------------------------
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

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_BINARY_DIR}/resources/models/Sponza
        $<TARGET_FILE_DIR:${TARGET_NAME}>/resources/models/Sponza
    COMMENT "Copying Sponza scene to output directory"
)






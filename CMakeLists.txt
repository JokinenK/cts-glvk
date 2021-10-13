cmake_minimum_required (VERSION 3.0)
project(cts-renderer)

option(CTS_BUILD_SAMPLES "Build samples" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if(CTS_BUILD_SAMPLES)
    add_subdirectory(contrib/glm)
    add_subdirectory(sample)

    list(APPEND INCLUDE_PATHS
        contrib/stb
        contrib/tinyobjloader
    )
endif(CTS_BUILD_SAMPLES)

add_subdirectory(contrib/glad)
add_subdirectory(contrib/vulkan)

list(APPEND HEADERS
)

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/command_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/command_dispatcher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/command_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/constants.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/descriptor_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/descriptor_set_layout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/descriptor_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/device_memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/fence.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/framebuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/fullscreen_texture.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/image_view.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/image.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/instance.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/physical_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/pipeline_layout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/pipeline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/query_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/render_pass.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/sampler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/semaphore.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/shader_module.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/swapchain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/type_mapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generic_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linear_allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pool_allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uuid.c
)

list(APPEND INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl
    ${CMAKE_SOURCE_DIR}/contrib/glad/include
    ${CMAKE_SOURCE_DIR}/contrib/vulkan/include
)

list(APPEND LIBRARIES
    glad
)

if(WIN32)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/win32/device_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/win32/surface.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_time.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_condition_variable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_thread.c
        
    )

    list(APPEND INCLUDE_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl/win32
        ${CMAKE_CURRENT_SOURCE_DIR}/include/win32
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/win32
    )

    list(APPEND LIBRARIES
        opengl32
        setupapi
    )
elseif(UNIX)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/platform_condition_variable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/platform_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/platform_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/platform_thread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/platform_surface.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/platform_time.c
    )

    list(APPEND INCLUDE_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl/posix
        ${CMAKE_CURRENT_SOURCE_DIR}/include/posix
        ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/posix
    )

    list(APPEND LIBRARIES
        gl
    )
endif(WIN32)



add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})
cmake_minimum_required (VERSION 3.0)
project(cts-renderer)

option(CTS_BUILD_SAMPLES "Build samples" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if(CTS_BUILD_SAMPLES)
    add_subdirectory(sample)
endif(CTS_BUILD_SAMPLES)

add_subdirectory(contrib/glad)
add_subdirectory(contrib/vulkan-headers)
add_subdirectory(contrib/spirv-cross)

list(APPEND HEADERS
)

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_dispatcher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/descriptor_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/descriptor_set_layout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/descriptor_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fence.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/framebuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gl_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_view.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instance.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/physical_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline_layout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/query_pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render_pass.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sampler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/semaphore.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shader_module.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/swapchain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/type_mapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generic_queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linear_allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pool_allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uuid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/icd.c
)

list(APPEND INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/contrib/glad/include
    ${CMAKE_SOURCE_DIR}/contrib/vulkan-headers/include
)

list(APPEND LIBRARIES
    glad
    spirv-cross-c
)

if(WIN32)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/device_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/gl_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/surface.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_time.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_condition_variable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32/platform_thread.c
        
    )

    list(APPEND INCLUDE_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/win32
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

add_definitions(-DLIBRARY_EXPORTS=1)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

add_custom_target(
    ${PROJECT_NAME}_MANIFEST ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/manifest.json $<TARGET_FILE_DIR:${PROJECT_NAME}>/manifest.json
) 
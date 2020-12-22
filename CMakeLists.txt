cmake_minimum_required (VERSION 3.0)
project(cts-renderer)

option(CTS_BUILD_TESTS "Build tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if(CTS_BUILD_TESTS)
    add_subdirectory(contrib/googletest)
endif(CTS_BUILD_TESTS)

add_subdirectory(contrib/glad)

list(APPEND HEADERS
)

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/renderer_impl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/renderer_proxy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/renderer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/type_mapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bump_allocator_proxy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bump_allocator.c
)

if(MSVC)
  list(APPEND SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win32/condition_variable.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win32/semaphore.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win32/thread.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win32/mutex.c
  )
elseif(UNIX)
  list(APPEND SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/posix/condition_variable.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/posix/semaphore.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/posix/thread.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/os/posix/mutex.c
  )
endif(MSVC)

if(WIN32)
  list(APPEND INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/os/win32
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win32
    ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl/win32
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/win32
)
elseif(UNIX)
  list(APPEND INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/os/posix
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os/posix
    ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl/posix
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl/posix
)
endif(WIN32)

list(APPEND INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl
    ${CMAKE_SOURCE_DIR}/contrib/glad/include
)

list(APPEND LIBRARIES
    glad
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})
cmake_minimum_required (VERSION 3.10)
project (glad)

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c
)

list(APPEND HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h
)

list(APPEND INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list(APPEND LIBRARIES
)

source_group("Source files" FILES ${SOURCES})
source_group("Header files" FILES ${HEADERS})

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})

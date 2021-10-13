cmake_minimum_required (VERSION 3.0)
project(cts-sample)

find_package(Vulkan REQUIRED)
add_subdirectory(contrib/glm)

list(APPEND SAMPLE_HEADERS
)

list(APPEND SAMPLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sample.cpp
)

list(APPEND SAMPLE_INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/tinyobjloader
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/opengl
    ${CMAKE_SOURCE_DIR}/contrib/glad/include
    ${CMAKE_SOURCE_DIR}/contrib/vulkan-headers/include
    ${CMAKE_SOURCE_DIR}/contrib/vulkan-loader
)

list(APPEND SAMPLE_LIBRARIES
    glm
    cts-renderer
    Vulkan::Vulkan
)

if(WIN32)
    list(APPEND SAMPLE_SOURCES
    )

    list(APPEND SAMPLE_INCLUDE_PATHS
        ${CMAKE_SOURCE_DIR}/include/opengl/win32
        ${CMAKE_SOURCE_DIR}/include/win32
    )

    list(APPEND SAMPLE_LIBRARIES
    )
elseif(UNIX)
    list(APPEND SAMPLE_SOURCES
    )

    list(APPEND SAMPLE_INCLUDE_PATHS
        ${CMAKE_SOURCE_DIR}/include/opengl/posix
        ${CMAKE_SOURCE_DIR}/include/posix
    )

    list(APPEND SAMPLE_LIBRARIES
    )
endif(WIN32)

list(APPEND SAMPLE_ASSET_FOLDERS
    shaders
    textures
    models
)

add_executable(${PROJECT_NAME} ${SAMPLE_SOURCES} ${SAMPLE_HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${SAMPLE_SOURCES} ${SAMPLE_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SAMPLE_INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SAMPLE_LIBRARIES})

foreach(ASSET_FOLDER ${SAMPLE_ASSET_FOLDERS})
    add_custom_target(
        ASSETS_${ASSET_FOLDER} ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${ASSET_FOLDER} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${ASSET_FOLDER}
    ) 
endforeach()

add_custom_target(
    ICD_BINARY ALL
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cts-renderer> $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_NAME:cts-renderer>
) 

add_custom_target(
    MANIFEST ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/manifest.json $<TARGET_FILE_DIR:${PROJECT_NAME}>/manifest.json
) 
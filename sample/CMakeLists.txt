cmake_minimum_required (VERSION 3.0)
project(cts-sample)

list(APPEND SAMPLE_HEADERS
)

list(APPEND SAMPLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sample.cpp
)

list(APPEND SAMPLE_INCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND SAMPLE_LIBRARIES
    cts-renderer
    glm
)

list(APPEND SAMPLE_ASSET_FOLDERS
    shaders
    textures
    models
)

add_executable(${PROJECT_NAME} ${SAMPLE_SOURCES} ${SAMPLE_HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${SAMPLE_SOURCES} ${SAMPLE_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SAMPLE_INCLUDE_PATHS} ${INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SAMPLE_LIBRARIES})

foreach(ASSET_FOLDER ${SAMPLE_ASSET_FOLDERS})
    add_custom_target(
        ASSETS_${ASSET_FOLDER} ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${ASSET_FOLDER} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${ASSET_FOLDER}
    ) 
endforeach()

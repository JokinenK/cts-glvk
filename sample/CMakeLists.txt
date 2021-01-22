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

list(APPEND SAMPLE_SHADERS
    shader.frag
    shader.vert
)

list(APPEND SAMPLE_RUNTIME_FOLDERS
    textures
)

add_executable(${PROJECT_NAME} ${SAMPLE_SOURCES} ${SAMPLE_HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${SAMPLE_SOURCES} ${SAMPLE_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SAMPLE_INCLUDE_PATHS} ${INCLUDE_PATHS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SAMPLE_LIBRARIES})

foreach(SHADER ${SAMPLE_SHADERS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()

foreach(RUNTIME_FOLDER ${SAMPLE_RUNTIME_FOLDERS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/${RUNTIME_FOLDER} $<TARGET_FILE_DIR:${PROJECT_NAME}>/${RUNTIME_FOLDER}
    )
endforeach()
cmake_minimum_required(VERSION 3.10.0)
project(UntitledTextRPG VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_subdirectory(thirdparty)

# Shaders
set(SHADER_PATH "${PROJECT_SOURCE_DIR}/shaders")
file(GLOB SHADER_SOURCE_FILES
    "${SHADER_PATH}/*.frag"
    "${SHADER_PATH}/*.vert"
)

# Assets
set(ASSET_PATH "${PROJECT_SOURCE_DIR}/assets")
file(GLOB ASSET_SOURCE_FILES
    "${ASSET_PATH}/*"
)

file(COPY ${SHADER_SOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(COPY ${ASSET_SOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

# Look into configure_file
# configure_file(TutorialConfig.h.in TutorialConfig.h)
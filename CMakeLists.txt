cmake_minimum_required(VERSION 3.19)

project(BlindEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Need conan.cmake")
endif()

set(BASE_CMAKE_BINARY_DIR "./build")
include(${BASE_CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(./include)

file(GLOB SRC_FILES
"${PROJECT_SOURCE_DIR}/src/*.h"
"${PROJECT_SOURCE_DIR}/src/*.cpp"
"${PROJECT_SOURCE_DIR}/src/*.c"
"${PROJECT_SOURCE_DIR}/src/*.cc"
)

file(GLOB COMPRESS_IMG_SRC_FILES
"${PROJECT_SOURCE_DIR}/tools/compress_img/*.cpp"
"${PROJECT_SOURCE_DIR}/tools/compress_img/*.c"
"${PROJECT_SOURCE_DIR}/tools/compress_img/*.cc"
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
add_executable(compress_img ${COMPRESS_IMG_SRC_FILES} "${PROJECT_SOURCE_DIR}/src/texture2d.cpp")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw3)
target_link_libraries(compress_img PRIVATE glfw3)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_20)
    
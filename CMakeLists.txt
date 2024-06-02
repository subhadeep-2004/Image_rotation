cmake_minimum_required(VERSION 3.5)

project(darotator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    main.cpp
)

# Include directories for Halide and its tools
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/include)
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/share/Halide/tools)

# Find packages
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenCV REQUIRED)

if(NOT JPEG_FOUND)
    message(FATAL_ERROR "JPEG library not found")
endif()

if(NOT PNG_FOUND)
    message(FATAL_ERROR "PNG library not found")
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link against Halide
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/lib/libHalide.a)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Link against Zlib
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)

# Link against JPEG
target_link_libraries(${PROJECT_NAME} JPEG::JPEG)

# Link against PNG
target_link_libraries(${PROJECT_NAME} PNG::PNG)

target_link_libraries(${PROJECT_NAME} dl)


target_link_libraries(${PROJECT_NAME} pthread)
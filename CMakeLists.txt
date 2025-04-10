cmake_minimum_required(VERSION 3.10)
project(FPSGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL and GLUT
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(FPSGame ${SOURCES})

# Link OpenGL and GLUT
target_link_libraries(FPSGame PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

# Copy shader files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

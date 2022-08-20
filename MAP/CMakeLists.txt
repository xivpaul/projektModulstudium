# Set the minimum version of CMake that can be used
cmake_minimum_required(VERSION 3.10)
# C++ Version required to scan file directory
set(CMAKE_CXX_STANDARD 17)
# Set the project name
project (MAP)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/Column.cpp
    src/CSV.cpp
    src/main.cpp
    src/mongoose.c    
    src/Server.cpp
    src/Value.cpp
    src/Visualization.cpp
    src/ScatterPlot.cpp
)

# Add an executable with the above sources
add_executable(map ${SOURCES})


# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(map
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(map ws2_32 )

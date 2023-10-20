cmake_minimum_required(VERSION 3.26)
project(FluidSimulation-SFML-CXX)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(FluidSimulation-SFML-CXX main.cpp)
target_link_libraries(FluidSimulation-SFML-CXX PRIVATE sfml-graphics)
target_compile_features(FluidSimulation-SFML-CXX PRIVATE cxx_std_17)
configure_file(gaussian.frag gaussian.frag COPYONLY)


install(TARGETS FluidSimulation-SFML-CXX)
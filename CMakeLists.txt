cmake_minimum_required(VERSION 3.30)
project(prog_graphique)

set(FLAGS -O2 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow -Wswitch-default -Wundef -Wno-unused -Wconversion -Wuseless-cast -Wno-missing-include-dirs)
macro(set_flags target)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${target} PUBLIC ${ALL_WARNINGS})
    endif ()
endmacro()

set(CMAKE_CXX_STANDARD 23)

add_executable(main
        main.cpp
        Sphere.cpp
        Sphere.h
        Vector.cpp
        Vector.h
        Ray.cpp
        Ray.h
        Scene.cpp
        Scene.h)
set_flags(main)

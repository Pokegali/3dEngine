cmake_minimum_required(VERSION 3.30)
project(prog_graphique)
find_package(OpenMP)

set(FLAGS -O2 -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow -Wswitch-default -Wundef -Wno-unused -Wconversion -Wuseless-cast -Wno-missing-include-dirs)
macro(set_flags target)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${target} PUBLIC ${ALL_WARNINGS})
    endif ()
endmacro()

set(CMAKE_CXX_STANDARD 23)

file(GLOB main_SRC CONFIGURE_DEPENDS "*.h" "*.cpp")
add_executable(main ${main_SRC})
set_flags(main)
if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

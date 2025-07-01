cmake_minimum_required(VERSION 3.10)
project(rupert)
enable_testing()


#### Compiler settings ####
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
add_compile_options(
    -O3
    -g3

    -Werror

    -Wpedantic
    -Wall
    -Wextra
    -Wdouble-promotion
    -Wformat-overflow=2
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-signedness
    -Winit-self
    -Wmain
    -Wmissing-include-dirs
    -Wshift-overflow=2
    -Wunused
    -Wuseless-cast
    -Wuninitialized
    -Wstringop-overflow=4

    -Wsuggest-attribute=pure
    -Wsuggest-attribute=const
    -Wsuggest-attribute=noreturn
    -Wmissing-noreturn
    -Wsuggest-attribute=malloc
    -Wsuggest-attribute=format
    -Wmissing-format-attribute
    -Wsuggest-attribute=cold

    -Warith-conversion
    -Wduplicated-branches
    -Wduplicated-cond
    -Wtrampolines
    -Wshadow
    -Wunsafe-loop-optimizations
    -Wpointer-arith
    -Wundef
    -Wconversion
    -Wlogical-op
    -Wredundant-decls
    #-Winline
    -Wlong-long
    -Wvector-operation-performance

    -Winvalid-imported-macros
    -Wnoexcept
    -Wreorder
    -Wredundant-tags
    -Weffc++
    -Wold-style-cast
    -Wsign-promo
    -Wextra-semi
)

#### Source files ####
include_directories(
    src
    src/interval
    src/geometry
    src/range
    src/queue
    src/flatbuffers
    src/global_solver
)
add_executable(rupert main.cpp)

#### Libraries ####
find_library(GMP_LIB gmp REQUIRED)
find_library(GMPXX_LIB gmpxx REQUIRED)
find_library(MPFR_LIB mpfr REQUIRED)
find_library(MPFI_LIB mpfi REQUIRED)
target_link_libraries(rupert PRIVATE ${GMP_LIB} ${GMPXX_LIB} ${MPFR_LIB} ${MPFI_LIB} /usr/lib/x86_64-linux-gnu/libflatbuffers.a)

#### Tests ####
find_package(Catch2 CONFIG REQUIRED)
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/test/*.cpp")
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
add_test(NAME tests COMMAND tests)

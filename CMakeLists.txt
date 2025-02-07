cmake_minimum_required(VERSION 3.10)
project(rupert)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_compile_options(
        # GCC/G++ Flags
        # Warnings
        -Wall                              # Enable all common warnings
        -Wextra                            # Enable extra warnings
        -Wpedantic                         # Enforce strict ISO C++
#        -Werror                            # Treat warnings as errors # TODO: Uncomment this line when I figure out how to ignore the flatbuffers header warnings
        -Weffc++                           # Warnings for violations of Effective C++ guidelines
        -Wconversion                       # Warn about implicit conversions
        -Wsign-conversion                  # Warn about sign conversions
        -Wshadow                           # Warn about shadowed variables
        -Wcast-qual                        # Warn about casts that remove type qualifiers
        -Wcast-align                       # Warn about pointer casts that increase alignment requirements
        -Wunused                           # Warn about unused variables
        -Woverloaded-virtual               # Warn about overloaded virtual functions
        -Wnon-virtual-dtor                 # Warn about non-virtual destructors
        -Wold-style-cast                   # Warn about C-style casts
        -Wsign-promo                       # Warn about implicit sign promotions
        -Wdouble-promotion                 # Warn about float being implicitly promoted to double
        -Wformat=2                         # Warn about printf/scanf format string issues
        -Wuninitialized                    # Warn about uninitialized variables
        -Winit-self                        # Warn about self initialization
        -Wpointer-arith                    # Warn about pointer arithmetic
        -Wno-unused-parameter              # Don't warn about unused parameters (often in virtual functions)
        -Wno-pessimizing-move              # Don't warn about avoidable mmoves

        # Debug Information
        -g3                                # Maximum debug information
        -fno-omit-frame-pointer            # Keep frame pointer for better debugging
        -fno-eliminate-unused-debug-types  # Keep debug info for unused types

        # Optimization Disabling
        -O0                                # Disable optimization
        -fno-inline                        # Disable inlining
        -fno-inline-functions              # Disable automatic inlining of functions
        -fno-inline-small-functions        # Disable automatic inlining of small functions
        -fno-default-inline                # Disable implicit inline for class members

        # Safety Features
        -fstack-protector-all              # Enable stack protection
        -fno-optimize-sibling-calls        # Disable tail call optimization
        -fvisibility=default               # Make all symbols visible
        -fPIE                              # Position Independent Executable

        # Sanitizers
        #        -fsanitize=address                 # Enable AddressSanitizer
        #        -fsanitize=undefined               # Enable UndefinedBehaviorSanitizer
        #        -fsanitize=leak                    # Enable LeakSanitizer

        # Standards and ABI
        -std=c++20                         # Use C++17 standard (adjust as needed)
        -D_GLIBCXX_DEBUG                   # Enable debug mode for standard library
        -D_GLIBCXX_DEBUG_PEDANTIC          # Enable pedantic debug mode
        -D_FORTIFY_SOURCE=2                # Enable runtime buffer overflow detection
)

find_package(OpenCV REQUIRED)

find_library(GMP_LIB gmp REQUIRED)
find_library(GMPXX_LIB gmpxx REQUIRED)

find_library(MPFR_LIB mpfr REQUIRED)

find_library(MPFI_LIB mpfi REQUIRED)


include_directories(
        src
        str/number
        src/interval
        src/vector
        src/geometry
        src/data
)

add_executable(rupert main.cpp)


#target_link_libraries(rupert PRIVATE ${OpenCV_LIBS} ${GMP_LIB} ${GMPXX_LIB} ${MPFR_LIB} ${MPFI_LIB})
target_link_libraries(rupert PRIVATE ${OpenCV_LIBS} ${GMP_LIB} ${GMPXX_LIB} ${MPFR_LIB} ${MPFI_LIB} /usr/lib/x86_64-linux-gnu/libflatbuffers.a)

cmake_minimum_required(VERSION 3.15)
project(StockMarketOrderBook VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard (or C++20 for better performance)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags for optimization and warnings
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Production-grade compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")

# Add optimization flags for low-latency performance
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )
    
    # Additional optimization flags for Release build
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(
            -flto                    # Link-time optimization
            -ffast-math             # Fast math operations
            -funroll-loops          # Loop unrolling
        )
    endif()
endif()

# Source files
set(SOURCES
    OrderBook.cpp
    MatchingEngine.cpp
    CLI.cpp
)

# Header files
set(HEADERS
    Order.h
    OrderBook.h
    MatchingEngine.h
    CLI.h
)

# Create library
add_library(orderbook_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(orderbook_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Main executable
add_executable(matching_engine main.cpp)
target_link_libraries(matching_engine PRIVATE orderbook_lib)

# Enable threading support if needed for future extensions
find_package(Threads REQUIRED)
target_link_libraries(matching_engine PRIVATE Threads::Threads)

# Installation
install(TARGETS matching_engine DESTINATION bin)
install(FILES ${HEADERS} DESTINATION include/orderbook)

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Optimization flags: ${CMAKE_CXX_FLAGS_RELEASE}")

cmake_minimum_required(VERSION 3.10)
project(ELK_CPP VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(GRAPH_SOURCES
    src/graph/graph.cpp
)

set(FORCE_SOURCES
    src/alg/force/force_layout.cpp
)

set(LAYERED_SOURCES
    src/alg/layered/lgraph.cpp
    src/alg/layered/layered_layout.cpp
    # Disabled advanced features to avoid compilation issues
    # These are not needed for netlistsvg
    # src/alg/layered/intermediate/self_loops.cpp
    # src/alg/layered/crossing/greedy_switch.cpp
    # src/alg/layered/crossing/median_heuristic.cpp
    # src/alg/layered/placement/network_simplex.cpp
    # src/alg/layered/placement/brandes_koepf.cpp
)

set(MRTREE_SOURCES
    # Disabled for netlistsvg - not needed
    # src/alg/mrtree/mrtree_layout.cpp
)

set(RECTPACKING_SOURCES
    # Disabled for netlistsvg - not needed
    # src/alg/rectpacking/rectpacking_layout.cpp
)

set(COMMON_SOURCES
    src/alg/common/edge_routing.cpp
    src/alg/common/port_constraints.cpp
    src/alg/common/label_placement.cpp
    src/alg/common/compaction.cpp
    src/alg/common/node_spacing.cpp
    src/alg/common/comments.cpp
    src/alg/common/incremental.cpp
)

set(CORE_SOURCES
    src/core/recursive_layout.cpp
    src/core/configuration.cpp
)

set(ALL_SOURCES
    ${GRAPH_SOURCES}
    ${CORE_SOURCES}
    ${FORCE_SOURCES}
    ${LAYERED_SOURCES}
    ${MRTREE_SOURCES}
    ${RECTPACKING_SOURCES}
    ${COMMON_SOURCES}
)

# Build as library
add_library(elk STATIC ${ALL_SOURCES})

# Installation
install(TARGETS elk
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/elk
    DESTINATION include
)

# Optional: Build example
option(BUILD_EXAMPLES "Build example programs" OFF)

if(BUILD_EXAMPLES)
    add_executable(elk_example examples/simple_example.cpp)
    target_link_libraries(elk_example elk)

    add_executable(elk_advanced examples/advanced_features.cpp)
    target_link_libraries(elk_advanced elk)

    add_executable(elk_test_all examples/test_all_features.cpp)
    target_link_libraries(elk_test_all elk)

    add_executable(netlistsvg_test examples/netlistsvg_test.cpp)
    target_link_libraries(netlistsvg_test elk)

    add_executable(complex_test examples/complex_test.cpp)
    target_link_libraries(complex_test elk)

    add_executable(multilayer_test examples/multilayer_test.cpp)
    target_link_libraries(multilayer_test elk)

    add_executable(json_test examples/json_test.cpp)
    target_link_libraries(json_test elk)
endif()

cmake_minimum_required(VERSION 3.31)

project(TDS_MODERN)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCES
        "${SRC_DIR}/app.cpp"
        "${SRC_DIR}/main.cpp"
        "${SRC_DIR}/config.cpp"
        "${SRC_DIR}/utils.cpp"
        "${SRC_DIR}/assets.cpp"
)

add_executable(TDS_MODERN ${SOURCES})

target_include_directories(TDS_MODERN PRIVATE ${SRC_DIR})

target_link_libraries(TDS_MODERN PRIVATE
        "E:/msys64/mingw64/lib/liburlmon.a"
        "E:/msys64/mingw64/lib/libwindowscodecs.a"
        "E:/msys64/mingw64/lib/libd2d1.a"
        -static
)

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static"
)

set_target_properties(TDS_MODERN PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)
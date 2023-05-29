cmake_minimum_required(VERSION 3.25)

project("serializer"
        VERSION 0.0.1
        LANGUAGES C CXX
        DESCRIPTION "C++ Binary serialization library"
)

include_directories(include)
include_directories(external)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)

add_subdirectory(src)

add_executable(runner main.cpp)

target_link_libraries(runner serializer)

# 启动单元测试
enable_testing()
# 进入单元测试
add_subdirectory(tests)
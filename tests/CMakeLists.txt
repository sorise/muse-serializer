cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

add_executable(serializerUnit BinarySerializerUnit.cpp)

add_executable(toolkitUnit utilUnit.cpp)

target_include_directories(serializerUnit
    PUBLIC
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(serializerUnit
    PUBLIC serializer
)

target_include_directories(toolkitUnit
    PUBLIC
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(toolkitUnit
    PUBLIC  serializer
)

add_test(
    NAME "serializerUnit"
    COMMAND serializerUnit
)

add_test(
    NAME "toolkitUnit"
    COMMAND toolkitUnit
)

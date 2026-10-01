cmake_minimum_required(VERSION 3.16)

project(clib LANGUAGES C)

file(GLOB_RECURSE SOURCES "src/*.h" "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} -lm)

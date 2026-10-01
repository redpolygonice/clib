cmake_minimum_required(VERSION 3.16)

project(clib LANGUAGES C)

add_executable(${PROJECT_NAME} test.c
    smap.c
    smap.h
    list.c
    list.h
    types.h
    nmap.c
    nmap.h
    nlist.c
    nlist.h
    slist.c
    slist.h
    sset.c
    sset.h
    nset.c
    nset.h)

target_link_libraries(${PROJECT_NAME} -lm)

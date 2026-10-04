cmake_minimum_required(VERSION 3.16)

project(clib LANGUAGES C)

option(ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)

file(GLOB_RECURSE SOURCES "src/*.h" "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} -lm)

if (ENABLE_SANITIZERS)
	target_compile_options(${PROJECT_NAME} PUBLIC
		-fsanitize=address,undefined
		-fno-omit-frame-pointer
		-fno-sanitize-recover=all
	)

	target_link_options(${PROJECT_NAME} PUBLIC
		-fsanitize=address,undefined
	)
endif()

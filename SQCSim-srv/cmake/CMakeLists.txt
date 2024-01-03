cmake_minimum_required(VERSION 3.18.4)

project(SQCSim-Server VERSION 0.1)

add_compile_options("-Wno-narrowing")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../out")

set(SQCSIM_COMMON_DIR "../../SQCSim-common/")

add_library(SQCSim-common
			"${SQCSIM_COMMON_DIR}blockinfo.cpp"
			"${SQCSIM_COMMON_DIR}bullet.cpp"
			"${SQCSIM_COMMON_DIR}chunk.cpp"
			"${SQCSIM_COMMON_DIR}netprotocol.cpp"
			"${SQCSIM_COMMON_DIR}opensimplex.cpp"
			"${SQCSIM_COMMON_DIR}player.cpp"
			"${SQCSIM_COMMON_DIR}transformation.cpp"
			"${SQCSIM_COMMON_DIR}world.cpp"
)

add_executable(SQCSim-server
			"../connection.cpp"
			"../server.cpp"
			"../main.cpp"
			)

target_link_libraries(SQCSim-server PUBLIC SQCSim-common)

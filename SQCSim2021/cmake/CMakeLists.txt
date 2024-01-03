cmake_minimum_required(VERSION 3.18.4)

project(SQCSim-Client VERSION 0.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED True)

set(CMAKE_BUILD_DIRECTORY "./build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

set(SQCSIM_COMMON_DIR "../../SQCSim-common/")
set(SQCSIM_EXTERN_DIR "../external/")

find_package(SFML COMPONENTS system window graphics REQUIRED)
find_package(DevIL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

find_library(IRRKLANG_LIBRARY
	NAMES libIrrKlang.so
	PATHS "${SQCSIM_EXTERN_DIR}/irrKlang-64bit-1.6.0/lib/linux-gcc-64/")

include_directories(
	${SQCSIM_EXTERN_DIR}/irrKlang-64bit-1.6.0/include/
			${SFML_INCLUDE_DIRS}
			${IL_INCLUDE_DIR}
			)


add_library(SQCSim-common
			"${SQCSIM_COMMON_DIR}boostinfo.cpp"
			"${SQCSIM_COMMON_DIR}blockinfo.cpp"
			"${SQCSIM_COMMON_DIR}bullet.cpp"
			"${SQCSIM_COMMON_DIR}chunk.cpp"
			"${SQCSIM_COMMON_DIR}netprotocol.cpp"
			"${SQCSIM_COMMON_DIR}opensimplex.cpp"
			"${SQCSIM_COMMON_DIR}player.cpp"
			"${SQCSIM_COMMON_DIR}transformation.cpp"
			"${SQCSIM_COMMON_DIR}world.cpp"
)

add_executable(SQCSim-client
			"../audio.cpp"
			"../booster.cpp"
			"../connector.cpp"
			"../engine.cpp"
			"../mesh.cpp"
			"../openglcontext.cpp"
			"../shader.cpp"
			"../skybox.cpp"
			"../texture.cpp"
			"../textureatlas.cpp"
			"../tool.cpp"
			"../vertexbuffer.cpp"
			"../renderer.cpp"
			"../remoteplayer.cpp"
			"../main.cpp"
			)

target_link_libraries(SQCSim-client PUBLIC SQCSim-common
			${IRRKLANG_LIBRARY}
			sfml-system
			sfml-window
			sfml-graphics
			GL
			GLU
			GLEW
			${IL_LIBRARIES}
			${ILU_LIBRARIES}
			)


add_executable(utrpg
main.cpp
engine.hpp
engine.cpp
engine_types.hpp
core_render.hpp
core_render.cpp
shader.hpp
renderable.hpp
renderable.cpp
game.hpp
game.cpp
util.hpp
util.cpp
player.hpp
stats.hpp
)
set_target_properties(utrpg PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(utrpg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(utrpg glm::glm glfw glad stbi)
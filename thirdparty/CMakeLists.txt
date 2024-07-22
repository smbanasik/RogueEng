add_subdirectory(glm_lib)
add_subdirectory(glfw-3.3.8)
add_subdirectory(glad)

add_library(stbi INTERFACE stb_image/stb_image.h)
set_target_properties(stbi PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(stbi INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stb_image)

add_library(ft_lib INTERFACE)
target_include_directories(ft_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/freetype/include)
target_link_directories(ft_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/freetype/lib)
target_link_libraries(ft_lib INTERFACE freetype)
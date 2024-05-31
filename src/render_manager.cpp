#include <render_manager.hpp>

#include <unordered_map>
#include <shader.hpp>
#include <renderable.hpp>

using namespace uteng_render;

void RenderManager::load_texture(const std::string& keyword, TexType type, const std::string& path, GLint param) {

}
void RenderManager::unload_texture(const std::string& keyword) {

}
void RenderManager::load_shader(const std::string& keyword, const std::string& vertex_path, const std::string& fragment_path) {

}
void RenderManager::unload_shader(const std::string& keyword) {

}
Texture* RenderManager::get_texture(const std::string& keyword) {

}
Shader* RenderManager::get_shader(const std::string& keyword) {

}
void RenderManager::append_dyn_model(const Model* model) {

}
void RenderManager::remove_dyn_model(unsigned int offset_from_end = 0) {

}
void RenderManager::append_static_model(const Model* model) {

}
void RenderManager::remove_static_model(unsigned int offset_from_end = 0) {

}
void RenderManager::clear_dyn() {

}
void RenderManager::clear_static() {

}
void RenderManager::reserve_dyn(unsigned int num_elems) {

}
void RenderManager::reserve_static(unsigned int num_elems) {

}
void RenderManager::trim_dyn() {

}
void RenderManager::trim_static() {

}
void RenderManager::draw() {

}
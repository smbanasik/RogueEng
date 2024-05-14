// 1/5/2024
// Spencer Banasik

// Contains all related functions for text
#ifndef UT_TEXT_HPP
#define UT_TEXT_HPP

#include <string>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <shader.hpp>

namespace uteng_render {

// What does the textmanager do:
// A textmanager contains everything needed to render text.
// It holds the buffers and textures used to render.
// 
// The interface is simple:
// When initializing, provide a list of fonts that will be used and the shader to render the text
// To prepare text for rendering, call render_text
// Then, run the draw command
// We'll also allow the option to free a font if the user guarantees that it will never be used.
//
// To prevent needless texture assembling, we'll store the text that needs to be rendered instead of requiring
// render_text to be called every loop.
// Then, we can allow removing rendered text, but only the last or an offset from the last
// This may be changed later to make things easier on the user.
// ------------------------------------------------------------------------------------------------

// Contains necessary information to transpose a font into vertices.
// https://learnopengl.com/img/in-practice/glyph.png
struct Character {
    glm::vec2 origin; // X and Y for character, normalized
    glm::ivec2 size;  // width and height
    glm::ivec3 padding; // left, top, and right offsets
};
// Contains the texture atlas of a font and the character information
struct Font {
    unsigned int texture = 0;
    unsigned int texture_width = 0;
    std::array<Character, 128> ascii; // TODO: change into something like a map, char to Character
};
// Useless?
struct CharVerts {
    glm::vec2 position, texture_coords;
};
// TODO: we need a struct that's an actual text block. Something that can hold the data of all of the verts, colors, and so on.

typedef std::vector<CharVerts> TextBlock; // TODO: id system with vectors
// TODO: add capabilities for multiple fonts
class TextManager {
public:

    TextManager();
    // TODO: I'd like to make these available when calling the constructor, but I'm afraid that to do that 
    // I'll need to have the Engine class have a pointer
    //TextManager(const FT_Library* ft, const std::string& font_path, const ShaderProgram* shader);
    ~TextManager();

    // Horrible, temporary, replace with a copy and an assignment constructor.
    void initialize(const FT_Library* ft, const std::string& font_path, const ShaderProgram* shader);

    // TODO: allow for font selection
    // TODO: allow for multi color text
    // Given text, a position on screen, and text details -> prepare text to be rendered on the creen
    void render_Text(const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color = { 0.0f, 0.0f, 0.0f });

    void draw(const glm::mat4& transform);

    void remove_last_text(const int offset = 0) {};

private:

    bool init_fonts(FT_Face& font); // TODO: configure fonts useds
    void add_text_draw() {}; // TODO: For every bit of data processed by render text, add it to what's drawn
    void convert_font_texture(FT_Face& font); // Given a face/font, convert it to a texture, then free the font

    unsigned int vao, vbo;
    const FT_Library* ft;
    FT_Face starter_font;
    Font starter_font_tex;
    const ShaderProgram* shader_ptr;

    std::vector<TextBlock> rendered_text;
};
}
#endif
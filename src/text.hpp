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

// BIG TODO LIST:
// - Get things thing scalable. A system for adding and removing texts.
// - Different fonts, different colors, different sizes and scales.
// - \n in a text should cause a newline to occur!
// - \t in a text should cause a tab to occur if it doesn't already!

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
    glm::vec2 tex_pos; // X and Y for character, normalized
    glm::ivec2 size;  // width and height
    glm::ivec3 padding; // left, top, and right offsets
};
// Contains the texture atlas of a font and the character information
struct Font {
    unsigned int texture;
    unsigned int texture_width;
    unsigned int texture_height;
    std::array<Character, 128> ascii; // TODO: change into something like a map, char to Character
};
// Data related to a text block
struct TextBlock {
    unsigned int id;
    float scale;
    glm::vec3 color;
    std::vector<float> verts;
};
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
    void render_text(const std::string& text, const glm::vec2& position, float scale = 1.0, const glm::vec3& color = { 1.0f, 1.0f, 1.0f });

    void draw(const glm::mat4& transform);

    void remove_last_text(const int offset = 0) {};

private:

    static constexpr unsigned int VERT_SIZE = 4;
    static constexpr unsigned int LAYOUT_POS = 0;
    static constexpr unsigned int LAYOUT_TEX = 1;
    static constexpr unsigned int OFFSET_POS = 0;
    static constexpr unsigned int OFFSET_TEX = 2;

    bool init_fonts(FT_Face& font); // TODO: configure fonts useds
    void add_text_draw() {}; // TODO: For every bit of data processed by render text, add it to what's drawn
    void convert_font_texture(FT_Face& font); // Given a face/font, convert it to a texture, then free the font
    void bind_buffer();

    unsigned int vao, vbo;
    const FT_Library* ft;
    FT_Face starter_font;
    Font starter_font_tex;
    TextBlock starter_block;
    const ShaderProgram* shader_ptr;

    std::vector<TextBlock> rendered_text;
};
}
#endif
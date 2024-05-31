// 1/5/2024
// Spencer Banasik

// Contains all related functions for text
#ifndef UT_TEXT_HPP
#define UT_TEXT_HPP

#include <string>
#include <array>
#include <vector>
#include <deque>
#include <unordered_map>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <shader.hpp>

// BIG TODO LIST:
// - Develop a system to assemble and bind *sometimes* rather than constantly, reduce calls
// - Different fonts, different colors, different sizes and scales.
// - \n in a text should cause a newline to occur!
// - \t in a text should cause a tab to occur if it doesn't already!
// - Rule of 3/5/0
// - Look for an alternative to an init function

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
    std::array<Character, 128> ascii; // TODO: change this to something scalable?
};
// Data related to a text block
struct TextBlock {
    unsigned int vert_id;
    std::string font_word;
};
typedef std::deque<TextBlock>::const_iterator TextBlockRef;
class TextManager {
public:

    TextManager();
    ~TextManager();

    void initialize(const FT_Library* ft, const ShaderProgram* shader);

    // Add a font, the keyword is used to access the font later.
    void add_font(const std::string& keyword, const std::string& font_path, const unsigned int pix_size = 48);

    void switch_selected_textBox(TextBlockRef block);
    void switch_selected_textBox_relToSel(const int offset, const bool forwards = true);
    void switch_selected_textBox_relToEnd(const int offset = 0);

    void remove_selected_textBox();
    void remove_relToSel(const int offset, const bool forwards = true);
    void remove_relToEnd(const int offset = 0);

    // TODO: pipeline
    // Format data
    // Render text (assembles it)
    // Set bind flag
    // If bind flag true, bind
    // draw

    // Set the color of future text blocks
    void set_text_color(const glm::vec3& text_color);
    // Set the position of future text blocks
    void set_text_position(const glm::ivec2& position);
    // Set the font of future text blocks
    void set_text_font(const std::string& keyword);
    // Set the scale of future text blocks
    void set_text_scale(const float scale);

    // Render text according to the set data, returns a reference to the generate block
    TextBlockRef render_text(const std::string& text);

    // Replace the selected text block according to the set data and new text
    void replace_text(const std::string& new_text);

    // Change the selected text block according to the set data
    //void modify_text(TextBlockRef text_block);
    
    // For every TextBlock, draw it.
    void draw(const glm::mat4& transform);

private:

    static constexpr unsigned int VERT_SIZE = 7;
    static constexpr unsigned int LAYOUT_POS = 0;
    static constexpr unsigned int LAYOUT_TEX = 1;
    static constexpr unsigned int LAYOUT_COL = 2;
    static constexpr unsigned int OFFSET_POS = 0;
    static constexpr unsigned int OFFSET_TEX = 2;
    static constexpr unsigned int OFFSET_COL = 4;

    TextManager(const TextManager& other) = delete;
    TextManager& operator=(const TextManager& other) = delete;
    TextManager(TextManager&& other) noexcept = delete;
    TextManager& operator=(TextManager&& other) noexcept = delete;

    // Initialize a font
    bool init_font(FT_Face& font, const std::string& font_path, unsigned int pix_size = 48);
    // Assemble the selected TextBlock into vertices
    void selected_textBlock_assemble(const std::string& text);
    //void handle_newLine();
    //void handle_tab();
    // TODO: allow for text codes to give specific text colors.
    
    // Given a face, convert it into our texture atlas
    void convert_font_texture(const std::string& key_word, FT_Face& font);
    // Bind our data to buffers
    void bind_buffer();
    std::deque<TextBlock>::iterator clear_textBlock(std::deque<TextBlock>::iterator& remove);

    unsigned int vao, vbo, ebo;
    bool bind_flag;
    const FT_Library* ft;
    const ShaderProgram* shader_ptr;
    struct {
        float scale;
        glm::vec3 color;
        glm::ivec2 position;
        std::string font_keyword;
    } sel_block_data;
    std::deque<TextBlock>::iterator selected_block;
    std::unordered_map<std::string, Font> font_pool;
    std::deque<TextBlock> rendered_text;
    // TODO: in an ideal world, this would be flattened,
    // but I *refuse* to go through that trouble right now.
    std::vector<std::vector<float>> verts;
    unsigned int temp_size;
};
}
#endif
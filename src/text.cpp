// 4/5/2024
// Spencer Banasik

#include <text.hpp>

#include <string>
#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <shader.hpp>

#include <iostream> // Debug

using namespace uteng_render;

TextManager::TextManager() {
    vao = vbo = ebo = 0;
    ft = nullptr;
    shader_ptr = nullptr;
    bind_flag = 0;
    sel_block_data.scale = 1.0f;
    sel_block_data.color = { 1.0f, 1.0f, 1.0f };
    sel_block_data.position = { 100, 100 };
    sel_block_data.font_keyword = "arial";


    temp_size = 0;



}
TextManager::~TextManager() {
    if (vao == 0)
        return;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void TextManager::initialize(const FT_Library* ft, const ShaderProgram* shader) {
    this->ft = ft;
    this->shader_ptr = shader;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
}

void TextManager::add_font(const std::string& keyword, const std::string& font_path, unsigned int pix_size) {
    FT_Face basic_font;
    if (!init_font(basic_font, font_path, pix_size))
        throw;
    convert_font_texture(keyword, basic_font);
    FT_Done_Face(basic_font);
}

void TextManager::switch_selected_textBox(TextBlockRef block) {
    selected_block = rendered_text.erase(block, block); // neat hack
}
void TextManager::switch_selected_textBox_relToSel(const int offset, const bool forwards) {
    if (forwards)
        selected_block += offset;
    else
        selected_block -= offset;
}
void TextManager::switch_selected_textBox_relToEnd(const int offset) {
    selected_block = rendered_text.end() - offset;
}
void TextManager::remove_selected_textBox() {

    selected_block = clear_textBlock(selected_block);
    
}
void TextManager::remove_relToSel(const int offset, const bool forwards) {

    auto removed_block = selected_block;
    if (forwards)
        removed_block += offset;
    else
        removed_block -= offset;

    clear_textBlock(removed_block);
}
void TextManager::remove_relToEnd(const int offset) {
    auto removed_block = rendered_text.end() - offset;
    clear_textBlock(removed_block);
}
void TextManager::set_text_color(const glm::vec3& text_color) {
    sel_block_data.color = text_color;
}
void TextManager::set_text_position(const glm::ivec2& position) {
    sel_block_data.position = position;
}
void TextManager::set_text_font(const std::string& keyword) {
    sel_block_data.font_keyword = keyword;
}
void TextManager::set_text_scale(const float scale) {
    sel_block_data.scale = scale;
}
std::deque<TextBlock>::const_iterator TextManager::render_text(const std::string& text) {
    TextBlock text_data;
    text_data.vert_id = verts.size();
    text_data.font_word = sel_block_data.font_keyword;
    rendered_text.push_back(text_data);
    selected_block = rendered_text.end() - 1;
    selected_textBlock_assemble(text);
    bind_flag = true;
    return selected_block;
}
void TextManager::replace_text(const std::string& new_text) {
    selected_block->font_word = sel_block_data.font_keyword;
    selected_textBlock_assemble(new_text);
    bind_flag = true;
}

// TODO: verts needs to be sorted by font. Not sure how to do this yet.
void TextManager::draw(const glm::mat4& transform) {

    if (!rendered_text.size())
        return;

    bool switch_point = false;
    unsigned int last_font = font_pool[rendered_text[0].font_word].texture;

    if (bind_flag)
        bind_buffer();

    shader_ptr->use();
    shader_ptr->set_mat4("transform", transform);
    shader_ptr->set_int("textu", 0);

    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, last_font);

    glDrawElements(GL_TRIANGLES, temp_size, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, temp_size);

    // TODO: WHOOPS NEED TO DO INDICES
    /*
    unsigned int startidx = 0, endidx = 0;
    for (auto it = rendered_text.begin(); it != rendered_text.end(); it++) {

        if (last_font != font_pool[it->font_word].texture) {
            glActiveTexture(GL_TEXTURE0);
            last_font = font_pool[it->font_word].texture;
            glBindTexture(GL_TEXTURE_2D, last_font);
            switch_point = true;
        }

        if (switch_point == true || (it + 1) == rendered_text.end()) {
            glDrawElements(GL_TRIANGLES, (endidx - startidx) * sizeof(float) * VERT_SIZE, GL_UNSIGNED_INT, (void*)(startidx * sizeof(float)));
            switch_point = false;
            startidx = endidx;
        }
        

        endidx += verts[it->vert_id].size(); // Increment the end idx by the amount of text we processed.
    }*/

    glBindVertexArray(0);
}

bool TextManager::init_font(FT_Face& font, const std::string& font_path, unsigned int pix_size) {
    if (FT_New_Face(*ft, font_path.c_str(), 0, &font))
        return false;
    FT_Set_Pixel_Sizes(font, 0, pix_size);
    return true;
}

void TextManager::selected_textBlock_assemble(const std::string& text) {

    std::vector<float> word;
    float x = sel_block_data.position.x;
    for (auto it = text.begin(); it != text.end(); it++) {

        Font font = font_pool[sel_block_data.font_keyword];
        Character c = font.ascii[*it];

        // TODO: scale multipliers here
        float xpos = x + c.padding.x;
        float ypos = sel_block_data.position.y - (c.size.y - c.padding.y);

        float width = c.size.x;
        float height = c.size.y;
        float atlas_width = width / font.texture_width;
        float atlas_height = height / font.texture_height;

        // 1 3
        // 0 2
        word.insert(word.end(),
            {
            xpos,         ypos,          c.tex_pos.x,               atlas_height, 
            sel_block_data.color.r, sel_block_data.color.g, sel_block_data.color.b,
            //xpos + width, ypos,          c.tex_pos.x + atlas_width, atlas_height,
            //sel_block_data.color.r, sel_block_data.color.g, sel_block_data.color.b,
            xpos,         ypos + height, c.tex_pos.x,               0.0f,
            sel_block_data.color.r, sel_block_data.color.g, sel_block_data.color.b,
            //xpos,         ypos + height, c.tex_pos.x,               0.0f,
            //sel_block_data.color.r, sel_block_data.color.g, sel_block_data.color.b,
            xpos + width, ypos,          c.tex_pos.x + atlas_width, atlas_height,
            sel_block_data.color.r, sel_block_data.color.g, sel_block_data.color.b,
            xpos + width, ypos + height, c.tex_pos.x + atlas_width, 0.0f,
            sel_block_data.color.r, sel_block_data.color.g, sel_block_data.color.b
            }
        );
        x += (c.padding.z >> 6);
    }
    verts.push_back(word);
}

void TextManager::convert_font_texture(const std::string& key_word, FT_Face& font) {

    int total_width = 0, max_height = 0;
    Font new_font;

    // TODO: we can try to do this in 1 loop instead of this and the one below.
    
    // For each character, scan through and capture the max height and total width
    for (unsigned char c = 0; c < 128; c++) {
        FT_Load_Char(font, c, FT_LOAD_RENDER);
        total_width += font->glyph->bitmap.width;
        if (max_height < font->glyph->bitmap.rows)
            max_height = font->glyph->bitmap.rows;
    }

    new_font.texture_width = total_width;
    new_font.texture_height = max_height;

    glGenTextures(1, &new_font.texture);
    glBindTexture(GL_TEXTURE_2D, new_font.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, total_width, max_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // For each character, create the dimensions in the array and place it into our array
    int current_width = 0;
    for (unsigned char c = 0; c < 128; c++) {
        FT_Load_Char(font, c, FT_LOAD_RENDER);

        new_font.ascii[c].tex_pos = { (1.0 * current_width) / total_width, 0.0f };
        new_font.ascii[c].size = { font->glyph->bitmap.width, font->glyph->bitmap.rows };
        new_font.ascii[c].padding = { font->glyph->bitmap_left, font->glyph->bitmap_top, font->glyph->advance.x };

        glTexSubImage2D(GL_TEXTURE_2D, 0, current_width, 0, font->glyph->bitmap.width, font->glyph->bitmap.rows,
            GL_RED, GL_UNSIGNED_BYTE, font->glyph->bitmap.buffer);
        current_width += font->glyph->bitmap.width;
    }

    this->font_pool.insert(std::make_pair(key_word, new_font));
}
void TextManager::bind_buffer() {

    unsigned int sum_size = 0;
    unsigned int idx_count = 0;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // TODO:
    // flatten our vertices
    // construct our indices

    // index order is 0, 2, 1, 1, 2, 3
    for (auto it = verts.begin(); it != verts.end(); it++) {
        sum_size += it->size();

        vertices.insert(vertices.end(), it->begin(), it->end());

        // TODO: construct our index
        // each index corresponds to a "vertex"
        // 4 verts -> 6 indices
        // vert size is 7, so 28 vert-elems -> 6 indices
        // So divide by 28, multiply by 6 to get the number of indices we need,
        // then for that count we insert X, X+2, X+1, X+1, X+2, X+3 and...
        // increment by 4?

        // For now we stick with arrays and add this later.

        unsigned int index_max = (it->size() / 28) * 6;
        for (int j = 0; j < index_max; j++) {

            std::array<unsigned int, 6> new_ind = { idx_count, idx_count + 2, idx_count + 1, idx_count + 1, idx_count + 2, idx_count + 3 };
            indices.insert(indices.end(), new_ind.begin(), new_ind.end());
            
            idx_count += 4;
        }

        
    }
    temp_size = indices.size();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(LAYOUT_POS, 2, GL_FLOAT, GL_FALSE, sizeof(float) * VERT_SIZE, (void*)OFFSET_POS);
    glEnableVertexAttribArray(LAYOUT_POS);

    glVertexAttribPointer(LAYOUT_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(float) * VERT_SIZE, (void*)(OFFSET_TEX * sizeof(float)));
    glEnableVertexAttribArray(LAYOUT_TEX);

    glVertexAttribPointer(LAYOUT_COL, 3, GL_FLOAT, GL_FALSE, sizeof(float) * VERT_SIZE, (void*)(OFFSET_COL * sizeof(float)));
    glEnableVertexAttribArray(LAYOUT_COL);

    glBindVertexArray(0);
}

std::deque<TextBlock>::iterator TextManager::clear_textBlock(std::deque<TextBlock>::iterator& remove) {

    unsigned int id = remove->vert_id;
    verts.erase(verts.begin() + id);
    rendered_text.erase(remove);
    bind_flag = true;
    return rendered_text.end() - 1;
}
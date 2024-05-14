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

// TODO: rule of 3/5/0.

// I HATE THIS I HATE THIS I HATE THIS
TextManager::TextManager() {
    // EMPTY! DO NOTHING!
    vao = vbo = 0;
    ft = nullptr;
    shader_ptr = nullptr;
    starter_font = nullptr;
}
/*
TextManager::TextManager(const FT_Library* ft, const std::string& font_path, const ShaderProgram* shader): ft(ft), shader_ptr(shader) {

    vao = 0;
    vbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Loaed Buffers with our temporary data
    float verts[] = {-1.f,-1.f, 0.0f,
                      1.f,-1.f, 0.0f,
                     -1.f, 1.f, 0.0f,
                     -1.f, 1.f, 0.0f,
                      1.f,-1.f, 0.0f,
                      1.f, 1.f, 0.0f};
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Initialize our FT_Face
    init_fonts(starter_font);

    // Fill starter_font_text
    //convert_font_texture(starter_font);

}
*/
TextManager::~TextManager() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void TextManager::initialize(const FT_Library* ft, const std::string& font_path, const ShaderProgram* shader) {
    this->ft = ft;
    this->shader_ptr = shader;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Loaed Buffers with our temporary data
    float verts[] = {-1.f,-1.f, 0.0f, 1.0f,
                      1.f,-1.f, 1.0f, 1.0f,
                     -1.f, 1.f, 0.0f, 0.0f,
                     -1.f, 1.f, 0.0f, 0.0f,
                      1.f,-1.f, 1.0f, 1.0f,
                      1.f, 1.f, 1.0f, 0.0f };
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Initialize our FT_Face
    init_fonts(starter_font);

    // Fill starter_font_text
    convert_font_texture(starter_font);
}

bool TextManager::init_fonts(FT_Face& font) {
    if (FT_New_Face(*ft, "../assets/fonts/arial.ttf", 0, &font)) {
        return false;
    }

    FT_Set_Pixel_Sizes(font, 0, 48);

    return true;
}

// TODO: refer to https://learnopengl.com/In-Practice/Text-Rendering
// and https://en.m.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_02
// this function is not well documented, nor is it really complete.
void TextManager::convert_font_texture(FT_Face& font) {

    int total_width = 0, max_height = 0;

    // TODO: we can try to do this in 1 loop instead of 2.
    // For each character, scan through and capture the max height and total width
    for (unsigned char c = 0; c < 128; c++) {
        FT_Load_Char(font, c, FT_LOAD_RENDER);
        total_width += font->glyph->bitmap.width;
        if (max_height < font->glyph->bitmap.rows)
            max_height = font->glyph->bitmap.rows;
    }

    std::cout << total_width << " " << max_height << "\n";

    //FT_Load_Char(font, 'B', FT_LOAD_RENDER);

    starter_font_tex.texture_width = total_width;
    glGenTextures(1, &starter_font_tex.texture);
    glBindTexture(GL_TEXTURE_2D, starter_font_tex.texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->glyph->bitmap.width, font->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, font->glyph->bitmap.buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, total_width, max_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // For each character, create the dimensions in the array and place it into our array
    int current_width = 0;
    for (unsigned char c = 0; c < 128; c++) {
        FT_Load_Char(font, c, FT_LOAD_RENDER);

        starter_font_tex.ascii[c].origin = { total_width / starter_font_tex.texture_width, 0.0f };
        starter_font_tex.ascii[c].size = { font->glyph->bitmap.width, font->glyph->bitmap.rows };
        starter_font_tex.ascii[c].padding = { font->glyph->bitmap_left, font->glyph->bitmap_top, font->glyph->advance.x};

        glTexSubImage2D(GL_TEXTURE_2D, 0, current_width, 0, font->glyph->bitmap.width, font->glyph->bitmap.rows,
            GL_RED, GL_UNSIGNED_BYTE, font->glyph->bitmap.buffer);
        current_width += font->glyph->bitmap.width;
    }

    FT_Done_Face(font);

}

// Not the same as a draw function! Instead, assemble a text block into using a draw function
void TextManager::render_Text(const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color) {

    // TODO: to start, given text and a starting position, use a font
    // and create an array of vertices (and texture coords) for 

}

void TextManager::draw(const glm::mat4& transform) {

    shader_ptr->use();
    //shader_ptr->set_mat4("transform", transform);
    shader_ptr->set_int("textu", 0);
    //shader_ptr->set_vec3("text_color", { 1.0f, 1.0f, 1.0f });

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, starter_font_tex.texture);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // TODO: for now, print out a square with the texture of our font.

    /*
    for (auto it = rendered_text.begin(); it != rendered_text.end(); it++) {

        shader_ptr->use();
        // TODO: set uniforms for shader
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);

        glBindTexture(GL_TEXTURE_2D, texture);

        // TODO: draw everything in our text queue
    }*/
}
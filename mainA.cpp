// Std. Includes
#include <iostream>
#include <map>
#include <string>
#include <windows.h>
#include <ctime>
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// GL includes
#include "Shader.h"
// Unicode
#include <unicode/unistr.h>

#define PATH(filename) ("C:\\Users\\baiqiang\\CLionProjects\\demo\\"#filename)

// Properties
const GLuint WIDTH = 1200, HEIGHT = 800;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<char16_t, Character>* Characters;
GLuint VAO, VBO;
// Load font as face
FT_Face face;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

Character getCharacter(FT_Face face);

// The MAIN function, from here we start our application and run the Game loop
int main() {
    // Init GLFW
    std::cout << "Init GLFW" << std::endl;
    Characters = new std::map<char16_t, Character>;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compile and setup the shader
    Shader shader(PATH(text/vert.glsl), PATH(text/frag.glsl));
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    if (FT_New_Face(ft, "C:\\Windows\\Fonts\\MSYH.ttc", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 24);
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    SYSTEMTIME start;
    GetSystemTime(&start);
    std::cout << "Start to load unicode " << start.wMinute << start.wSecond << start.wMilliseconds << std::endl;
    for (char16_t i = 0x4E00; i < 0x9FA5; i++) {
        char16_t c = i;
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Store character for later use
        (*Characters)[c] = getCharacter(face);
    }
    GetSystemTime(&start);
    std::cout << "End to load unicode " << start.wMinute << start.wSecond << start.wMilliseconds << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    SYSTEMTIME sys;
    const char *tpl = "%4d/%02d/%02d %02d:%02d:%02d";
    const size_t len = 64;
    char time[len] = "";

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GetLocalTime(&sys);
        snprintf(time, len, tpl, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);

        RenderText(shader, "FreeType是一个能够用于加载字体并将他们渲染到位图以及提供多种字体相关的操作的软件开发库", 25.0f, 200.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader, time, 540.0f, 570.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f));

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glfwTerminate();
    return 0;
}

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    GLfloat originX = x;
    // Activate corresponding render state
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    UnicodeString ucs = UnicodeString::fromUTF8(StringPiece(text.c_str()));

    Character ch;
    for (int i = 0; i < ucs.length(); i++) {
        char16_t c = ucs[i];
        if (Characters->find(c) == Characters->end()) {
            if (FT_Load_Char(face, static_cast<wchar_t>(ucs[i]), FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // Store character for later use
            ch = getCharacter(face);
            (*Characters)[c]= ch;
        } else {
            ch = (*Characters)[c];
        }

        GLint line = 0;

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        if (x - originX > 500) {
            x = originX;
            line++;
            y -= line * h * 1.5;
        }
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Character getCharacter(FT_Face f) {

    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            f->glyph->bitmap.width,
            f->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            f->glyph->bitmap.buffer
    );
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return {
            texture,
            glm::ivec2(f->glyph->bitmap.width, f->glyph->bitmap.rows),
            glm::ivec2(f->glyph->bitmap_left, f->glyph->bitmap_top),
            static_cast<GLuint>(f->glyph->advance.x)
    };
}
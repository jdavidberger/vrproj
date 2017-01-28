#include "DrawText.h"
#include <iostream>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include "GlUtils.h"

#include <opencv2/core.hpp>
#include "Resources/Resources.h"

#include "shared/Matrices.h"

struct Character {
	GLuint     textureID;  // ID handle of the glyph texture
	cv::Vec2i Size;       // Size of glyph
	cv::Vec2i Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

GLuint VAO = -1, VBO = -1;
GLuint fontShader = -1;
GLint textColor = -1;
GLint textProj = -1;

std::map<GLchar, Character> Characters;

static void init() {
	static bool didInit = false;
	if (!didInit) {
		didInit = true; 

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		FT_Library ft;
		FT_Face face;
		std::string prefix("../assets/");
		std::string pwd = ".";
		std::string fontPath = "C:/Windows/Fonts/times.ttf";
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, 48);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

		for (GLubyte c = 0; c < 128; c++) {
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Now store character for later use
			Character character = {
				texture,
				cv::Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				cv::Vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));
		}
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		fontShader = CompileGLShader("Font", Resources::Font_vs, Resources::Font_fs);

		textColor = glGetUniformLocation(fontShader, "textColor");
		textProj = glGetUniformLocation(fontShader, "projection");
		if (textColor == -1 || textProj == -1) {
			printf("Unable to find uniform in shader\n");
			return;
		}
	}
}

struct DrawText_p {
	DrawText_p() {
		init();
	}
	void operator()(const Matrix4& tx, float _x, float y, float scale, const std::string& text) {		
		float x = _x;
		// Iterate through all characters		
		glUseProgram(fontShader);
		glUniform3f(textColor, 1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(textProj, 1, GL_TRUE, tx.val);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
		
		for (auto c = text.begin(); c != text.end(); c++) {
			if (*c == '\n') {
				y -= 64 * scale;
				x = _x; 
				continue; 
			}
			Character ch = Characters[*c];

			GLfloat xpos = x + ch.Bearing[0] * scale;
			GLfloat ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

			GLfloat w = ch.Size[0] * scale;
			GLfloat h = ch.Size[1] * scale;
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
			glBindTexture(GL_TEXTURE_2D, ch.textureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};
GlDrawText::GlDrawText() {}
GlDrawText::~GlDrawText(){}

DrawText_p* GlDrawText::data()
{
	if (!p)
		p = std::unique_ptr<DrawText_p>(new DrawText_p());
	return p.get();	
}

void GlDrawText::operator()(const Matrix4& tx, float x, float y, float scale, const std::string& text) {	
	(*data())(tx, x, y, scale, text);
}
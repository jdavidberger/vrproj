#include <string>
#include <memory>
#include "shared/Matrices.h"

struct DrawText_p; 
struct GlDrawText {
	std::unique_ptr<DrawText_p> p;
    GlDrawText();
    ~GlDrawText();
	DrawText_p* data(); 
    void operator()(const Matrix4& tx, float x, float y, float scale, const std::string& text);
}; 

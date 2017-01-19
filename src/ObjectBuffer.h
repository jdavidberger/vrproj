#include <GL/glew.h>
#include <vector>
#include "Polytope.h"
#include <shared/Matrices.h>
#include <tuple>

struct VertexData {
	VertexData(); 
	VertexData(const Vector4& v) : vertex(v) {}
	VertexData(float x, float y, float z, float w, float r, float g, float b);
	Vector4 vertex;
	Vector3 rgb; 
	Vector4 normal;
};

typedef std::vector<VertexData> Surface;
typedef std::tuple<VertexData, VertexData> Edge;

struct ObjectBuffer {
	typedef cv::Matx<float, 5, 5> matrix_t;
	GLuint m_glSceneVertBuffer = 0;
	GLuint m_unSceneVAO = 0;
	size_t m_length = 0;

	GLuint m_glEdgeVertBuffer = 0;
	GLuint m_unEdgeVAO = 0;
	size_t m_edgeLength = 0;

	bool m_visible = true;
	matrix_t m_tx = matrix_t::eye();

	void SetTx(const matrix_t& tx);
	ObjectBuffer(const Polytype_<4>::Polytope& shape);
	void SetBuffer(GLuint& vao, GLuint& buffer, size_t& length, const std::vector<float>& vertices);
	void Draw(bool drawSurfaces = true, bool drawEdges = true) const;
	~ObjectBuffer();

	cv::Vec3f edgeVertexColor(const cv::Vec4f& vertex) {
		cv::Vec3f color(.1, .1, .1);
		color[0] = (vertex[3] + 1.) / 2.;
		return color; 
	}
private:
	typedef Polytype_<4>::edge_t edge_t; 
	void SetupEdges(const std::vector< edge_t >& edges);
	void SetupSurfaces(const std::vector< Polytype_<4>::Surface >& surfaces);
};


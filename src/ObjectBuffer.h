#include <GL/glew.h>
#include <vector>
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

	mutable std::vector<ObjectBuffer> children;

	GLuint m_glSceneVertBuffer = 0;
	GLuint m_unSceneVAO = 0;
	GLuint m_type = 0;
	size_t m_length = 0;
	bool m_visible = true;
	Matrix5 m_tx = Matrix5::eye();
	void SetTx(const Matrix5& tx);
	ObjectBuffer(const std::vector<Surface>& surfaces);
	ObjectBuffer(const std::vector<Edge>& edges);
	ObjectBuffer(const std::vector<float>& vertices, GLuint type);
	void SetBuffer(const std::vector<float>& vertices, GLuint type);
	void Draw() const;
	~ObjectBuffer();
};
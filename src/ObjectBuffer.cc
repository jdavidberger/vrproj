#include "ObjectBuffer.h"
#include "shared/Matrices.h"
#include <assert.h>

void ObjectBuffer::SetTx(const Matrix5 & tx)
{
	m_tx = tx; 
}
static void AddVertex(std::vector<float>& data, const VertexData& vd) {	
	data.push_back(vd.vertex[0]);
	data.push_back(vd.vertex[1]);
	data.push_back(vd.vertex[2]);
	data.push_back(vd.vertex[3]);
	data.push_back(vd.rgb[0]);
	data.push_back(vd.rgb[1]);
	data.push_back(vd.rgb[2]);
	data.push_back(vd.vertex[0]);
	data.push_back(vd.vertex[1]);
	data.push_back(vd.vertex[2]);
	data.push_back(vd.vertex[3]);

}
ObjectBuffer::ObjectBuffer(const std::vector<Edge>& edges)
{
	std::vector<float> data; 
	for (auto& e : edges) {
		AddVertex(data, std::get<0>(e));
		AddVertex(data, std::get<1>(e));
	}
	SetBuffer(data, GL_LINES);
}

static Vector4 CalculateNormalFromBasis(const Vector4& U, const Vector4& V, const Vector4& W) {

	Matrix3 a(U[1], V[1], W[1],
		U[2], V[2], W[2],
		U[3], V[3], W[3]);
	a = a.t();

	Matrix3 b(U[0], V[0], W[0],
		U[2], V[2], W[2],
		U[3], V[3], W[3]);
	b = b.t();

	Matrix3 c(U[0], V[0], W[0],
		U[1], V[1], W[1],
		U[3], V[3], W[3]);
	c = c.t();

	Matrix3 d(U[0], V[0], W[0],
		U[1], V[1], W[1],
		U[2], V[2], W[2]);
	d = d.t();

	auto e = Vector4(cv::determinant(a), cv::determinant(b), cv::determinant(c), cv::determinant(d));
	return cv::normalize(e);	
}

static Vector4 CalculateNormal(const Vector4& p1, const Vector4& p2, const Vector4& p3) {
	auto mid = (p1 + p2 + p3) / 3;
	auto U = p2 - p1;
	auto V = p3 - p1;
	U = cv::normalize(U);
	V = cv::normalize(V);

	auto W = Vector4(0,0,0,1);
	auto e = CalculateNormalFromBasis(U, V, W);
	if (isnan( cv::norm(e) ) || cv::norm(e) < .5) {
		W = Vector4(0, 0, 1, 0);
		e = CalculateNormalFromBasis(U, V, W);
	}
	if (isnan(cv::norm(e)) || cv::norm(e) < .5) {
		W = Vector4(0, 1, 0, 0);
		e = CalculateNormalFromBasis(U, V, W);
	}
	if (isnan(cv::norm(e)) || cv::norm(e) < .5) {
		W = Vector4(1, 0, 0, 0);
		e = CalculateNormalFromBasis(U, V, W);
	}
	return e;
}

ObjectBuffer::ObjectBuffer(const std::vector<Surface>& surfaces)
{
	std::vector<float> data;
	
	std::vector<Edge> edges;

	for (auto& e : surfaces) {
		auto a = e[0];
		auto b = e[1];
		for (size_t i = 2;i < e.size();i++) {
			auto c = e[i];
			auto n = CalculateNormal(a.vertex, b.vertex, c.vertex);
			if(cv::norm(n) < 0.99)
				assert(cv::norm(n) > 0.99);
			if ( cv::norm(a.vertex + n) < cv::norm(a.vertex - n))
				n = -n;
			a.normal = b.normal = c.normal = n;
			edges.emplace_back((a.vertex + b.vertex + c.vertex) / 3., n + (a.vertex + b.vertex + c.vertex) / 3.);
			std::get<0>(edges.back()).rgb = Vector3(1, 1, 1); 
			std::get<1>(edges.back()).rgb = Vector3(1, 1, 1);
			AddVertex(data, a);
			AddVertex(data, b);
			AddVertex(data, c);
			a = b;
			b = c;
		}
	}
	SetBuffer(data, GL_TRIANGLES);
	//children.emplace_back(edges);
	
//	SetBuffer(data, GL_LINE_STRIP);
}

ObjectBuffer::ObjectBuffer(const std::vector<float>& vertices, GLuint type) 
{
	SetBuffer(vertices, type); 
}

void ObjectBuffer::SetBuffer(const std::vector<float>& vertices, GLuint type)
{
	m_type = type;
	m_length = vertices.size() / (sizeof(VertexData) / sizeof(float));

	glGenVertexArrays(1, &m_unSceneVAO);
	glBindVertexArray(m_unSceneVAO);

	glGenBuffers(1, &m_glSceneVertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_glSceneVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	GLsizei stride = sizeof(VertexData);
	uintptr_t offset = 0;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (const void *)offset);
	offset += sizeof(Vector4);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offset);
	offset += sizeof(Vector3);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void *)offset);
	
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void ObjectBuffer::Draw() const
{
	if (m_visible) {
		glBindVertexArray(m_unSceneVAO);
		glDrawArrays(m_type, 0, m_length);
		glBindVertexArray(0);

		for (size_t i = 0;i < children.size();i++) {
			children[i].Draw();
		}

	}
}

ObjectBuffer::~ObjectBuffer()
{
	/*
	glDeleteBuffers(1, &m_glSceneVertBuffer);
	if (m_unSceneVAO != 0)
	{
		glDeleteVertexArrays(1, &m_unSceneVAO);
	}
	*/
}

VertexData::VertexData()
{
}

VertexData::VertexData(float x, float y, float z, float w, float r, float g, float b) : vertex(x,y,z,w), rgb(r,g,b)
{

}

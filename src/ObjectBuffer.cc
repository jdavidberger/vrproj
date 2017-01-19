#include "ObjectBuffer.h"
#include "shared/Matrices.h"
#include <assert.h>

void ObjectBuffer::SetTx(const Matrix5 & tx) {
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
void ObjectBuffer::SetupEdges(const std::vector< edge_t >& edges) {
	std::vector<float> data;
	for (auto& e : edges) {
		VertexData d = e.first; 
		d.rgb = edgeVertexColor(e.first); 
		AddVertex(data, d);
		d = e.second;
		d.rgb = edgeVertexColor(e.second);
		AddVertex(data, d);
	}
	SetBuffer(m_unEdgeVAO, this->m_glEdgeVertBuffer, this->m_edgeLength, data);
}
ObjectBuffer::ObjectBuffer(GLuint programId, const Polytype_<4>::Polytope& shape) : m_programId(programId) {
	SetupEdges(shape.edges);
	SetupSurfaces(shape.surfaces);
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

void ObjectBuffer::SetupSurfaces(const std::vector<Polytype_<4>::Surface>& surfaces)
{
	std::vector<float> data;

	std::vector<Edge> edges;

	for (auto& e : surfaces) {
		auto n = CalculateNormal(e.triangulation[0], e.triangulation[1], e.triangulation[2]);

		//if (cv::norm(n) < 0.99)
			//assert(cv::norm(n) > 0.99);
		if (cv::norm(e.triangulation[0] + n) < cv::norm(e.triangulation[0] - n))
			n = -n;

		for (auto tript : e.triangulation) {
			VertexData v = tript;
			v.normal = n;
			v.rgb = e.color;
			AddVertex(data, v);
		}
	}
	SetBuffer(m_unSceneVAO, m_glSceneVertBuffer, m_length, data);
	//children.emplace_back(edges);

	//	SetBuffer(data, GL_LINE_STRIP);
}
/*
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
*/
void ObjectBuffer::SetBuffer(GLuint& vao, GLuint& buffer, size_t& length, 
	const std::vector<float>& vertices)
{	
	length = vertices.size() / (sizeof(VertexData) / sizeof(float));

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
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

void ObjectBuffer::Draw(bool drawSurfaces, bool drawEdges) const
{	
	if (drawSurfaces) {
		glBindVertexArray(m_unSceneVAO);
		glDrawArrays(GL_TRIANGLES, 0, m_length);
	}
	if (drawEdges) {
		glBindVertexArray(m_unEdgeVAO);
		glDrawArrays(GL_LINES, 0, m_edgeLength);
	}
	glUseProgram(0);
	glBindVertexArray(0);	
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

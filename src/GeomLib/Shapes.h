#include "Polytope.h"
#include <set>
#include <array>

static std::vector< Polygon_<2> > CubeSurfaces(float width, float height) {
	std::vector< Polygon_<2> > rtn; 
	float w2 = width / 2., h2 = height / 2.;
	cv::Vec2f A = cv::Vec2f(-w2, -h2);
	cv::Vec2f B = cv::Vec2f( w2, -h2);
	cv::Vec2f C = cv::Vec2f( w2,  h2);
	cv::Vec2f D = cv::Vec2f(-w2,  h2);

	rtn.push_back(Polygon_<2>({ A, B, C, D })); 
	return rtn;
}

static std::vector< Polygon_<3> > CubeSurfaces(float width, float height, float depth) {
	static std::vector< Polygon_<3> > rtn;
	
	cv::Vec3f A = cv::Vec3f(-1, -1, -1);
	cv::Vec3f B = cv::Vec3f(1, -1, -1);
	cv::Vec3f C = cv::Vec3f(1, 1, -1);
	cv::Vec3f D = cv::Vec3f(-1, 1, -1);

	cv::Vec3f E = cv::Vec3f(-1, -1, 1);
	cv::Vec3f F = cv::Vec3f(1, -1, 1);
	cv::Vec3f G = cv::Vec3f(1, 1, 1);
	cv::Vec3f H = cv::Vec3f(-1, 1, 1);

	rtn.push_back(Polygon_<3>({ F, E, H, G })); // Front
	rtn.push_back(Polygon_<3>({ A, B, C, D }));
	rtn.push_back(Polygon_<3>({ G, H, D, C }));
	rtn.push_back(Polygon_<3>({ B, A, E, F }));
	rtn.push_back(Polygon_<3>({ E, A, D, H }));
	rtn.push_back(Polygon_<3>({ B, F, G, C }));

	return rtn;
}

static std::vector< cv::Vec4f > HypercubeIndices() {
	std::vector< cv::Vec4f > vertices;

	vertices.push_back(cv::Vec4f(1, 1, 1, 1));
	vertices.push_back(cv::Vec4f(-1, 1, 1, 1));
	vertices.push_back(cv::Vec4f(-1, -1, 1, 1));
	vertices.push_back(cv::Vec4f(1, -1, 1, 1));

	vertices.push_back(cv::Vec4f(1, -1, -1, 1));
	vertices.push_back(cv::Vec4f(-1, -1, -1, 1));
	vertices.push_back(cv::Vec4f(-1, 1, -1, 1));
	vertices.push_back(cv::Vec4f(1, 1, -1, 1));

	vertices.push_back(cv::Vec4f(1, 1, 1, -1));
	vertices.push_back(cv::Vec4f(-1, 1, 1, -1));
	vertices.push_back(cv::Vec4f(-1, -1, 1, -1));
	vertices.push_back(cv::Vec4f(1, -1, 1, -1));

	vertices.push_back(cv::Vec4f(1, -1, -1, -1));
	vertices.push_back(cv::Vec4f(-1, -1, -1, -1));
	vertices.push_back(cv::Vec4f(-1, 1, -1, -1));
	vertices.push_back(cv::Vec4f(1, 1, -1, -1));

	return vertices;
}

static std::vector< Polygon_<4> > CubeSurfaces(float width, float height, float depth, float wepth) {
	std::vector< Polygon_<4> > rtn;
	std::vector< cv::Vec4f > vertices = HypercubeIndices();
	std::set<std::set<size_t>> idxs;

	cv::Matx<float, 4, 4> scale = cv::Matx<float, 4, 4>::zeros();
	cv::Vec<float, 4> diag(width/2., height / 2., depth / 2., wepth / 2.);
	for (int i = 0;i < 4;i++)
		scale(i, i) = diag(i);

	for (size_t i = 0;i < vertices.size();i++) {
		auto& vi = vertices[i];
		for (size_t j = 0;j < vertices.size();j++) {
			auto& vj = vertices[j];
			if (i == j) continue;
			if (cv::norm(vi - vj) > 2.01) continue;

			for (size_t k = 0;k < vertices.size();k++) {
				auto& vk = vertices[k];
				if (cv::norm(vi - vk) > 2.01) continue;

				for (size_t l = 0;l < vertices.size();l++) {
					auto& vl = vertices[l];
					if (cv::norm(vl - vk) > 2.01 || cv::norm(vl - vj) > 2.01) continue;

					std::set<size_t> s{ i, j, k, l };
					if (s.size() != 4) continue;
					if (idxs.find(s) != idxs.end()) continue;
					idxs.insert(s);

					Polygon_<4> surface;
					surface.emplace_back(scale * vi);//(vi, vj, vl, vk, 1, 1);
					surface.emplace_back(scale * vj);
					surface.emplace_back(scale * vl);
					surface.emplace_back(scale * vk);

					rtn.push_back(surface);
				}
			}
		}
	}



	return rtn;
}

Polytype_<2>::Polytope CubeSurfaces_(const std::array<float, 2>& params) {		
	return Polytype_<2>::Polytope(CubeSurfaces(params.at(0), params.at(1)));
}

Polytype_<3>::Polytope CubeSurfaces_(const std::array<float, 3>& params) {
	return Polytype_<3>::Polytope(CubeSurfaces(params[0], params[1], params[2]));
}

Polytype_<4>::Polytope CubeSurfaces_(const std::array<float, 4>& params) {
	return Polytype_<4>::Polytope(CubeSurfaces(params[0], params[1], params[2], params[3]));
}
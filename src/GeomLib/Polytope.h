#pragma once
#include <stdlib.h>
#include "Polygon.h"
#include <iostream>
#include <utility>
#include <unordered_set>
#include "shared/Matrices.h"
std::vector<cv::Vec2f> Triangulate2d(const std::vector<cv::Vec2f>& input);

template <size_t N>
struct Polytype_ {

	static cv::Vec<float, N> TransformPt(const cv::Matx<float, N + 1, N + 1>& tx, const cv::Vec<float, N>& pt) {
		cv::Vec<float, N + 1> in;
		for (size_t i = 0; i < N; i++)
			in[i] = pt[i];
		in[N] = 1.0;
		auto out = tx * in; 
		cv::Vec<float, N> rtn;
		for (size_t i = 0; i < N; i++)
			rtn[i] = out[i] / out[N]; 
		return rtn; 
	}

	struct Surface {
		cv::Vec3f color;
		Polygon_<N> boundary; 
		std::vector< cv::Vec<float, N> > triangulation;	

		Surface Transform(const cv::Matx<float, N+1, N + 1>& tx) const {
			Surface rtn;
			rtn.color = color; 
			rtn.boundary.reserve(boundary.size()); 
			rtn.triangulation.reserve(triangulation.size());
			for (auto& b : boundary) rtn.boundary.push_back(TransformPt(tx, b));
			for (auto& b : triangulation) rtn.triangulation.push_back(TransformPt(tx, b));
			return rtn; 
		}

		void ComputeTriangulation() {
			cv::Vec<float, N> mean; 
			Polygon_<N> centeredBoundary;
			for (auto& pt : boundary) {
				mean += pt;
			}
			mean = mean / (float)boundary.size();
			for (auto& pt : boundary) {
				centeredBoundary.push_back(pt - mean);
			}
			
			cv::Mat H(centeredBoundary.size(), N, CV_32F, &centeredBoundary[0]);
			cv::Mat S, U, Vt;
			cv::SVD::compute(H, S, U, Vt, cv::SVD::FULL_UV);
			
			std::vector<cv::Vec2f> planePts;

			for (auto&pt : centeredBoundary) {
				cv::Mat npt = Vt * cv::Mat(pt);
				planePts.push_back(cv::Vec2f(npt.at<float>(0, 0), npt.at<float>(1, 0)));
			}
			
			auto tris = Triangulate2d(planePts);
			triangulation.reserve(tris.size());
			for (auto& tri : tris) {
				cv::Vec<float, N> in = cv::Vec<float, N>::all(0);
				in[0] = tri[0];
				in[1] = tri[1];
				cv::Mat_<float> newmat = Vt.t() * cv::Mat(in);
				cv::Vec<float, N> newpt = cv::Vec<float, N>((float*)newmat.data);
				triangulation.push_back(newpt + mean);
			}
		}

		template <size_t M>
		static Polygon_<N> Project(const Polygon_<M>& boundary) {
			Polygon_<N> rtn;
			for (auto& pt : boundary) {
				cv::Vec<float, N> newpt = {};
				for (size_t i = 0; i < std::min(N, M); i++)
					newpt(i) = pt(i);
				rtn.emplace_back(newpt);
			}
			return rtn;
		}
		
		template <size_t M>
		Surface(const Polygon_<M>& boundary) : boundary(Project(boundary)) {
			ComputeTriangulation();
		}
		Surface() {}
		Surface(const Polygon_<N>& boundary) : boundary(boundary) {
			ComputeTriangulation();
		}
	};

	template <size_t M>
	static std::vector< Surface > MakeSurfaces(const std::vector< Polygon_<M> >& polygons) {
		std::vector< Surface > rtn;
		rtn.reserve(polygons.size());
		for (auto& it : polygons)
			rtn.emplace_back(it);
		return rtn;
	}
	typedef std::pair< cv::Vec<float, N>, cv::Vec<float, N> > edge_t;
	struct EdgeComparator {
		bool operator()(const cv::Vec<float, N>& a, const cv::Vec<float, N>& b) {
			for (unsigned i = 0;i < N;i++) {
				if (a[i] != b[i])
					return a[i] < b[i]; 
			}
			return false; 
		}
		bool operator()(const edge_t& a, const edge_t& b) {
			if(a.first == b.first)
				return this->operator()(a.second, b.second);
			return this->operator()(a.first, b.first);
		}
	};
	struct Polytope {
		
		std::vector< Surface > surfaces; 
		std::vector< edge_t > edges; 
		void Add(const Polytope& p) {
			surfaces.insert(surfaces.end(), p.surfaces.begin(), p.surfaces.end());
			edges.insert(edges.end(), p.edges.begin(), p.edges.end());
		}
		void ComputeEdges() {
			std::set<edge_t, EdgeComparator> edges; 

			for (auto& surface : surfaces) {
				auto lastPt = surface.boundary.front(); 
				for (size_t i = 1;i < surface.boundary.size();i++) {					
					edges.insert(std::make_pair(lastPt, surface.boundary[i])); 
					lastPt = surface.boundary[i]; 
				}
				edges.insert(std::make_pair(lastPt, surface.boundary.front())); 
			}
			this->edges.insert(this->edges.end(), edges.begin(), edges.end());
		}
		template <size_t M>
		Polytope(const std::vector< Polygon_<M> >& polygons) : Polytope(MakeSurfaces(polygons)) {}
		Polytope(const std::vector< Surface >& surfaces) : surfaces(surfaces) {
			ComputeEdges();	
		}
		Polytope Transform(const cv::Matx<float, N +1, N + 1>& tx) const {
			Polytope rtn; 
			rtn.surfaces.reserve(surfaces.size());
			rtn.edges.reserve(edges.size());

			for (auto& s : surfaces) rtn.surfaces.push_back(s.Transform(tx));
			for (auto& s : edges) {
				rtn.edges.push_back( std::make_pair(TransformPt(tx, s.first), 
													TransformPt(tx, s.second)));
			}

			return rtn;
		}
		Polytope() {}
	};
};

extern template struct Polytype_<3>;
extern template struct Polytype_<4>;


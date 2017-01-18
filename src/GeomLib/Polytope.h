#pragma once
#include <stdlib.h>
#include "Polygon.h"
#include <iostream>
#include <utility>
#include <unordered_set>

std::vector<cv::Vec2f> Triangulate2d(const std::vector<cv::Vec2f>& input);

template <size_t N>
struct Polytype_ {
	struct Surface {
		Polygon_<N> boundary; 
		std::vector< cv::Vec<float, N> > triangulation;		
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

			std::cerr << H << std::endl << S << std::endl << U << std::endl << Vt << std::endl;

			std::cerr << Vt.t() << std::endl;
			
			std::vector<cv::Vec2f> planePts;

			for (auto&pt : centeredBoundary) {
				cv::Mat npt = Vt * cv::Mat(pt);
				planePts.push_back(cv::Vec2f(npt.at<float>(0, 0), npt.at<float>(1, 0)));
				std::cerr << Vt * cv::Mat(pt) << std::endl;
			}
			
			auto tris = Triangulate2d(planePts);
			triangulation.resize(tris.size());
			for (auto& tri : tris) {
				cv::Vec<float, N> in = cv::Vec<float, N>::all(0);
				in[0] = tri[0];
				in[1] = tri[1];
				cv::Mat_<float> newmat = Vt.t() * cv::Mat(in);
				cv::Vec<float, N> newpt = cv::Vec<float, N>((float*)newmat.data);
				std::cerr << newmat << std::endl << newpt << std::endl;
				triangulation.push_back(newpt + mean);
			}
		}

		Surface(const Polygon_<N>& boundary) : boundary(boundary) {
			ComputeTriangulation();
		}
	};
	static std::vector< Surface > MakeSurfaces(const std::vector< Polygon_<N> >& polygons) {
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
		Polytope(const std::vector< Polygon_<N> >& polygons) : Polytope(MakeSurfaces(polygons)) {}
		Polytope(const std::vector< Surface >& surfaces) : surfaces(surfaces) {
			ComputeEdges();
		}
	};
};

extern template struct Polytype_<3>;
extern template struct Polytype_<4>;


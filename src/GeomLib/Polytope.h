#include <stdlib.h>
#include "Polygon.h"
#include <iostream>
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

	struct Polytope {
		Polytope(const std::vector< Surface >&);
	};
};

extern template struct Polytype_<3>;
extern template struct Polytype_<4>;


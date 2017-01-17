#include "Polytope.h"
#define REAL float
#define VOID void
#define ANSI_DECLARATORS
extern "C" {
#include "triangle.h"
}
#undef REAL

std::vector<cv::Vec2f> Triangulate2d(const std::vector<cv::Vec2f>& planePts)
{

	triangulateio in = {}, mid = {}, out = {};

	in.numberofpoints = planePts.size();
	in.pointlist = const_cast<float*>(&planePts.front()[0]);

	triangulate("Vz", &in, &mid, &out);

	std::vector<cv::Vec2f> tris;
	for (size_t i = 0;i < mid.numberoftriangles * mid.numberofcorners;i++) {
		float* pt = mid.trianglelist[i] * 2 + mid.pointlist; 
		tris.emplace_back(pt[0], pt[1]);
	}
	return tris;
}

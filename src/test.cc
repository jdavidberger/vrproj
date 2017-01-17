#include <iostream>
#include "Polytope.h"

#include "shared/Matrices.h"

int main(int argc, char *argv[]) {
	{
		Matrix5 f = Matrix5::eye();
		f(0, 0) = 2;
		f(0, 1) = 2;

		auto val = f * Vector5(1, 2, 3, 4, 5);
		std::cerr << val << std::endl;
	}
	{
		Matrix5 f = Matrix5::eye();
		f(0, 0) = 2;
		f(1, 0) = 2;

		auto val = f * Vector5(1, 2, 3, 4, 5);
		std::cerr << val << std::endl;
	}

	double A = 2, B = 3, C = 4, D = 5, E = 6;
	auto makePlanePt = [&] (double x, double y) {
		return cv::Vec3f(x, y, (A * x + B * y + D) / C);
	};

	auto make4PlanePt = [&](double x, double y, double z) {
		return cv::Vec4f(x, y, z, (A * x + B * y + C * z + E) / D);
	};
	Polygon_<3> boundary = {
		makePlanePt(0, 0),
		makePlanePt(0, 1),
		makePlanePt(1, 0),
		makePlanePt(1, 1),
		makePlanePt(.5, .5) };

	Polytype_<3>::Surface three(boundary);

	Polygon_<4> boundary4 = {
		make4PlanePt(0, 0, 1),
		make4PlanePt(0, 1, 2),
		make4PlanePt(1, 0, 3),
		make4PlanePt(1, 1, 4),
		make4PlanePt(.5, .5, 5) };

	Polytype_<4>::Surface four(boundary4);

	return 0;
}

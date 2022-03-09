#pragma once

#include <cmath>

constexpr auto one_six = 0.1666666666666667; // value of 1/6

// struct to store vertex:
struct Vertex {
	double x;
	double y;
	double z;

	Vertex() :
		x(0), y(0), z(0){}

	Vertex(double X, double Y, double Z) :
		x(X), y(Y), z(Z) {}
};


class Volume {
public:

	// calculate the determinant of one triangulated face
	static double calculate_determinant(Vertex& v1, Vertex& v2, Vertex& v3)
	{
		/*
		* matrix: 
		* x1 y1 z1
		* x2 y2 z2
		* x3 y3 z3
		* D = x1 y2 z3 + y1 z2 x3 + z1 y3 x2 - z1 y2 x3 - y1 x2 z3 - x1 y3 z2
		* 
		* matrix:
		* v1.x v1.y v1.z
		* v2.x v2.y v2.z
		* v3.x v3.y v3.z
		*/
		return
			v1.x * v2.y * v3.z +
			v1.y * v2.z * v3.x +
			v1.z * v3.y * v2.x -
			v1.z * v2.y * v3.x -
			v1.y * v2.x * v3.z -
			v1.x * v3.y * v2.z;
	}

	/*
	* calculate the "volume" of each solid
	* Vertices_one_solid: [[v1, v2, v3], [v4, v5, v6], ... []]
	*/
	static double calculate_volume(
		std::vector<std::vector<Vertex>>& Vertices_one_solid)
	{
		double sum_det = 0;

		for (auto& v_each_triangulated_face : Vertices_one_solid)
		{
			/*
			* each v_each_triangulated_face: [vertex1, vertex2, vertex3]
			* oriented: CCW or CW but faces must be the same orientation
			*/
			sum_det += calculate_determinant(v_each_triangulated_face[0], v_each_triangulated_face[1], v_each_triangulated_face[2]);
		}

		/*
		* because absolute value is used in the fomula
		* the orientation of each triangulated face can be CCW or CW
		* but all the faces' orientations should be associated(all CCW or all CW)
		*/
		return one_six * abs(sum_det); // (1/6)*|sum_det|
	}
};

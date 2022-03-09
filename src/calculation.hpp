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

	// calculate the "volume" of each (triangulated)face
	static double calculate_volume_each_triangulated_face(
		Vertex& v1, Vertex& v2, Vertex& v3)
	{
		double det = calculate_determinant(v1, v2, v3);
		return one_six * abs(det);
	}
};

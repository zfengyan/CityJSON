#pragma once

#include <cmath>
#include <vector>

constexpr auto one_six = 0.1666666666666667; // value of 1/6
constexpr auto _INFINITE_ = 9999; // value of infinite

// class to store vertex:
class Vertex {
public:
	double x;
	double y;
	double z;

	int vid; // might be used
public:
	Vertex() :
		x(0), y(0), z(0), vid(0){}

	Vertex(double X, double Y, double Z, int id) :
		x(X), y(Y), z(Z), vid(id){}

	Vertex(double X, double Y, double Z):
		x(X), y(Y), z(Z), vid(0) {}
};


// class to represent normal vector
class Vector3d : public Vertex 
{
public:
	Vector3d() :
		Vertex() {}

	Vector3d(double X, double Y, double Z):
		Vertex(X, Y, Z){}

	static Vector3d cross(Vector3d& v1, Vector3d& v2) {
		return Vector3d();
	}
};


// class related to volume
class Volume {
public:
	/*
	* calculate the determinant of one triangulated face
	*/
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
	static double calculate_volume_one_shell(
		std::vector<std::vector<Vertex>>& v_one_shell)
	{
		double sum_det = 0;

		for (auto& v_each_triangulated_face : v_one_shell)
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


// class to store the roofsurface
class RoofSurface {
public:
	std::string BuildingPart_id; // belongs to which BuildingPart
	std::string type;

	int boundaries_index; // also semantics_values_index
	int semantics_surfaces_index;
		
	std::string orientation;

	std::vector<Vertex> RoofVertices; // store the 3 vertices of thie roof surface
public:
	RoofSurface():
		BuildingPart_id("null"),
		type("RoofSurface"),
		boundaries_index(_INFINITE_),
		semantics_surfaces_index(_INFINITE_),
		orientation("null")
	{
		RoofVertices.reserve(3);
	}


	/*
	* calculate and assign the orientation
	* use y-axis as the North vector: [0,1,0]
	*/
	static std::string calculate_orientation(RoofSurface& roof)
	{
		return "null";
	}
};


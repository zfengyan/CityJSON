#pragma once

#include <cmath>
#include <vector>

constexpr auto one_six = 0.1666666666666667; // value of 1/6
constexpr auto _INFINITE_ = 9999; // value of infinite
constexpr auto epsilon = 1e-8; // threshold


class Vertex;
class Vector3d;
class Volume;
class RoofSurface;


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

public:

	// get cross product
	static Vector3d cross(Vector3d& v1, Vector3d& v2) {
		return Vector3d(
			(v1.y * v2.z - v1.z * v2.y),
			(v1.z * v2.x - v1.x * v2.z),
			(v1.x * v2.y - v1.y * v2.x)
		);
	}

	// get normal vector of one face
	// RoofVertices: store 3 vertices of one face, v1, v2, v3 should be oriented as CCW from outside
	static Vector3d find_normal(std::vector<Vertex>& RoofVertices) {
		
		// define two vectors of this roof surface
		// v1: starts from vertex[0], ends at vertex[1]
		// v2: starts from vertex[1], ends at vertex[2]
		Vector3d v1(
			(RoofVertices[1].x - RoofVertices[0].x),
			(RoofVertices[1].y - RoofVertices[0].y),
			(RoofVertices[1].z - RoofVertices[0].z)
		);

		Vector3d v2(
			(RoofVertices[2].x - RoofVertices[1].x),
			(RoofVertices[2].y - RoofVertices[1].y),
			(RoofVertices[2].z - RoofVertices[1].z)
		);

		return cross(v1, v2);
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

	std::vector<Vertex> RoofVertices; // store the 3 vertices of thie roof surface(v1, v2, v3, CCW from outside)
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

public:

	/*
	* assign quadrant according to x y coordinates
	* @return: 1, 2, 3, 4 -- indicating four quadrants
	*/
	static int assign_quadrant(double x, double y)
	{
		//if ((x - 0) > epsilon && (y - 0) > epsilon))return 1; // 1-th quadrant
		//else if((x - 0) > epsilon && (y - 0) > epsilon))
	}


	/*
	* calculate and assign the orientation
	* use y-axis as the North(North vector: [0, 1, 0])
	*/
	static std::string calculate_orientation(RoofSurface& roof)
	{
		// get the normal vector of curent roof surface
		Vector3d& normal = Vector3d::find_normal(roof.RoofVertices);
		
		// get the absolute normal
		Vector3d abs_normal(
			abs(normal.x),
			abs(normal.y),
			abs(normal.z)
		);

		// situation cannot use alpha = arctan(x/y)
		// orientaion is either East or West(using 2d coordinates x, y to estimate the orientation)
		// in the orientation, only 8 values + "horizontal", E, W, N, S should be replaced with proper values(like EN)
		if ((abs_normal.y - 0) <= epsilon) // y = 0
		{
			if ((normal.x - 0) > epsilon)return "EN"; // can also return "ES"
			else return "WN"; // can also return "WS"
		}

		// first assign the quadrant according to x,y signs
		
		// situation can use alpha = arctan(x/y)
		// double alpha = atan(abs_normal.x / abs_normal.y);

		return "null";
	}
};


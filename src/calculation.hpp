#pragma once

#include <cmath>
#include <vector>



#define one_six 0.166666667 // value of 1/6
#define _INFINITE_ 9999 // value of infinite
#define epsilon 1e-8 // threshold
#define quadrant_pi_radius 0.785398163 // pi/4 used for determine the prientation


class ErrorObject;

class Vertex;
class Vector3d;
class Volume;
class RoofSurface;
class Triangle;



class ErrorObject {
public:
	std::string building_id; // Building id
	std::string building_part_id; // BuildingPart id
	std::string error_type; 

	int error_code; // error code

	std::vector<int> boundaries_index; // error faces' indexes with consecutive points in the boundaries list
public:
	ErrorObject():
		building_id("null"),
		building_part_id("null"),
		error_type("null"),
		error_code(0)
	{}
};



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



// class to store triangle
class Triangle {
public:
	Vertex v0;
	Vertex v1;
	Vertex v2;
	double t_area;
public:
	Triangle() :t_area(0) {};

	void print() // print triangle
	{
		std::cout << "v" << " " << v0.x << " " << v0.y << " " << v0.z << '\n';
		std::cout << "v" << " " << v1.x << " " << v1.y << " " << v1.z << '\n';
		std::cout << "v" << " " << v2.x << " " << v2.y << " " << v2.z << '\n';
	}
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

	/*
	* assign quadrant according to x y coordinates
	* @return: 1, 2, 3, 4 -- indicating four quadrants
	* if ">" not ">=", use: x+epsilon > 0, the same as "<"
	* compared with 0 can directly use > or >= ?
	*/
	static int assign_quadrant(double x, double y)
	{
		if (x > epsilon || abs(x) < epsilon) { // x > 0 or x = 0 : quadrant 1 or 4
			if (y > epsilon || abs(y) < epsilon)return 1; // y > 0 or y = 0 : quadrant 1
			else return 4; // y < 0 : quadrant 4
		}
		else { // quadrant 2 or 3
			if (y > epsilon || abs(y) < epsilon)return 2; // y > 0 or y = 0 : quadrant 2
			else return 3; // y < 0: quadrant 3
		}
	}


	/*
	* get a vector of the surface(according to exterior surface)
	*/
	static Vector3d get_vector(std::vector<Vertex>& exteriorSurface)
	{
		return Vector3d(
			(exteriorSurface[1].x - exteriorSurface[0].x),
			(exteriorSurface[1].y - exteriorSurface[0].y),
			(exteriorSurface[1].z - exteriorSurface[0].z)
		);
	}


	// get cross product
	static Vector3d cross(Vector3d& v1, Vector3d& v2) {
		return Vector3d(
			(v1.y * v2.z - v1.z * v2.y),
			(v1.z * v2.x - v1.x * v2.z),
			(v1.x * v2.y - v1.y * v2.x)
		);
	}


	// get normal vector
	static Vector3d get_normal(Triangle& t) {

		Vector3d v1(
			(t.v1.x - t.v0.x),
			(t.v1.y - t.v0.y),
			(t.v1.z - t.v0.z)
		);

		Vector3d v2(
			(t.v2.x - t.v1.x),
			(t.v2.y - t.v1.y),
			(t.v2.z - t.v1.z)
		);

		return cross(v1, v2);

	}


	// 2x2 matrix:
	// x1 y1
	// x2 y2
	static double det_2x2(Vertex& v1, Vertex& v2)
	{
		return (v1.x * v2.y - v1.y * v2.x);
	}


	//ccw from outside check
	//@return:
	//true -- ccw from outside
	//false -- cw or colinear from outside
	//only for roof surface -- if projected to 2d
	//if for fround surface -- if projected to 2d then CCW from outside correponds to CW in xy plane
	static bool roof_vertices_ccw_check(Vertex& p, Vertex& q, Vertex& r)
	{
		double area = det_2x2(p, q) + det_2x2(q, r) + det_2x2(r, p);
		if (area > epsilon)return true;
		else return false;
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
	std::string roof_id; // unique id of a roof surface object

	int boundaries_index; // also semantics_values_index
	int semantics_surfaces_index;
		
	std::string orientation; // indicates orientaion
	double area; // indicates area
	double area_tri;

	std::vector<Vertex> exteriorSurface; // store the vertices of exterior face, should be CCW from outside
	std::vector<std::vector<Vertex>> interiorSurfaces; // store the vertices of interior faces, should be CW(for each interior face) from outside
	
	std::vector<std::vector<Vertex>> triangles; // store the triangles of one surface(from triangulated file)
	std::vector<Triangle> triangle_list; // store the triangles as Triangle class
	Vector3d roof_normal; // normal vector, might be useful
public:
	RoofSurface():
		BuildingPart_id("null"),
		type("RoofSurface"),
		roof_id("null"),
		boundaries_index(_INFINITE_),
		semantics_surfaces_index(_INFINITE_),
		orientation("null"),
		area(0),
		area_tri(0)
	{}

public:

	///*
	//* calculate and assign the orientation of roof surface
	//* use y-axis as the North(North vector: [0, 1, 0])
	//*/
	//static std::string calculate_orientation_one_roof(RoofSurface& roof)
	//{
	//	// use exterior surface of current roof surface to get the normal vector
	//	Vector3d& normal = Vector3d::get_normal(roof.exteriorSurface);
	//	roof.roof_normal = normal;

	//	if (abs(normal.x - 9999) < epsilon) // vector: (9999, 0, 0) -- indicates no proper normal found
	//	{
	//		std::cout << "no proper normal found in buildingpart: " << '\n';
	//		std::cout << roof.BuildingPart_id << '\n';
	//		return "null";
	//	}

	//	// situation cannot use alpha = arctan(x/y)
	//	// orientaion is either East or West(using 2d coordinates x, y to estimate the orientation)
	//	// in the orientation, only 8 values + "horizontal", E, W, N, S should be replaced with proper values(like EN)
	//	if (abs(normal.y) < epsilon) // y = 0
	//	{
	//		if (normal.x > epsilon)return "EN"; // x > 0 (y = 0)
	//		else if (abs(normal.x) < epsilon)return "horizontal"; // x = 0 (y = 0)
	//		else return "WN"; // x < 0 (y = 0)
	//	}

	//	// situation can use alpha = arctan(x/y)
	//	// double alpha = atan(abs_normal.x / abs_normal.y);
	//	// 
	//	// use normal to decide the quadrant
	//	// use abs_normal to calculate the angle
	//	// 
	//	// first assign the quadrant according to x,y signs
	//	int quadrant = Vector3d::assign_quadrant(normal.x, normal.y);

	//	// use abs_normal to get the angle(radius, value of atan(): [-pi, pi])
	//	double radius_angle = atan(abs(normal.x) / abs(normal.y));

	//	switch (quadrant)
	//	{
	//	case 1: // 1-th quadrant
	//		if (radius_angle > quadrant_pi_radius + epsilon) // > pi/4
	//			return "EN";
	//		else return "NE";
	//		break;
	//		
	//	case 2: // 2-th quadrant
	//		if (radius_angle > quadrant_pi_radius + epsilon) // > pi/4
	//			return "WN";
	//		else return "NW";
	//		break;

	//	case 3: // 3-th quadrant
	//		if (radius_angle > quadrant_pi_radius + epsilon) // > pi/4
	//			return "WS";
	//		else return "SW";
	//		break;

	//	case 4: // 4-th quadrant
	//		if (radius_angle > quadrant_pi_radius + epsilon) // > pi/4
	//			return "ES";
	//		else return "SE";
	//		break;

	//	default:
	//		break;
	//	}

	//	return "null"; // if no matching found, return null
	//}



	/*
	* calculate the area of roof surface(2d)
	*/
	static double calculate_area_2d(RoofSurface& roof)
	{
		// First calculate the area of exterior(one exterior face for each roof surface)
		long N_exterior = (long)roof.exteriorSurface.size();
		double sum_exterior_det = 0;
		for (long i = 0; i != N_exterior - 1; ++i)
		{
			sum_exterior_det += Vector3d::det_2x2(roof.exteriorSurface[i], roof.exteriorSurface[i + 1]);
		}
		sum_exterior_det += Vector3d::det_2x2(roof.exteriorSurface[N_exterior - 1], roof.exteriorSurface[0]);
		double exterior_area = 0.5 * abs(sum_exterior_det);


		// Second calculate the area of all interior faces(may be multiple interior faces for each roof surface)
		double interior_area = 0; // interior area for all interior faces
		for (auto& one_interior : roof.interiorSurfaces)
		{
			long N_one_interior = (long)one_interior.size();
			double sum_one_interior_det = 0;
			for (long i = 0; i != N_one_interior - 1; ++i)
			{
				sum_one_interior_det += Vector3d::det_2x2(one_interior[i], one_interior[i + 1]);
			}
			sum_one_interior_det += Vector3d::det_2x2(one_interior[N_one_interior - 1], one_interior[0]);
			double one_interior_area = 0.5 * abs(sum_one_interior_det);

			interior_area += one_interior_area;
		}


		// area for this roof surface: exterior - interior
		double area_2d = exterior_area - interior_area;
		return area_2d > epsilon ? area_2d : 0;
	}



	/*
	* calculate the area of roof surface(3d)
	* area must be >= 0
	*/
	static double calculate_area_3d(RoofSurface& roof)
	{
		Vector3d& surface_vector = Vector3d::get_vector(roof.exteriorSurface); //  get a vector in this surface

		double length_x = abs(surface_vector.x);
		double length_y = abs(surface_vector.y);
		double length_z = abs(surface_vector.z);
		if (length_z < epsilon)return calculate_area_2d(roof); // z = 0, horizontal surface

		double length_2d = sqrt(length_x * length_x + length_y * length_y);
		if (!length_2d) // vertical surface
		{
			std::cout << "alert: vertical roofsurface, BuildingPartid: " << roof.BuildingPart_id << '\n';
			std::cout << "index in the geometry -> boundaries list: " << roof.boundaries_index << '\n';
		}			
		
		double radius_angle = atan(length_z / length_2d);

		// area_3d * cos(radius_angle) = area_2d
		if (abs(cos(radius_angle)) < epsilon){
			std::cout << "alert: calculation warning: " << roof.BuildingPart_id << '\n';
			std::cout << "index in the geometry -> boundaries list: " << roof.boundaries_index << '\n';
		}
		else {
			return calculate_area_2d(roof) / abs(cos(radius_angle));
		}
		return 0;
	}
};

